#include "function.h"
extern "C"{
#include <math.h>
}

ErrorType Function::calculate(float x, float &result)
{
    StackArray<float> stack;
    float left;
    float right;

    // Probably rude, but ok
    result = NAN;

    for (uint8_t i = 0; i < FUNCTION_MAX_TOKENS; i++)
    {
        if (isConstant(function[i]))
        {
            stack.push(constants[function[i]]);
        }
        else if (isOperator(function[i]))
        {
            if (getParameters(function[i]) > stack.count())
            {
                return (ErrorType)2;/*ErrorType::NOT_ENOUGH_PARAMETERS*/;
            }

            switch (function[i])
            {
            case TOKEN_ADD:
                right = stack.pop();
                left = stack.pop();
                stack.push(left + right);
                break;
            case TOKEN_SUB:
                right = stack.pop();
                left = stack.pop();
                stack.push(left - right);
                break;
            case TOKEN_MUL:
                right = stack.pop();
                left = stack.pop();
                stack.push(left * right);
                break;
            case TOKEN_DIV:
                right = stack.pop();
                left = stack.pop();
                stack.push(left / right);
                break;
            case TOKEN_POW:
                right = stack.pop();
                left = stack.pop();
                stack.push(pow(left, right));
                break;
            case TOKEN_MOD:
                right = stack.pop();
                left = stack.pop();
                stack.push(fmod(left, right));
                break;
            case TOKEN_NEG:
                stack.push(-stack.pop());
                break;
            }
        }
        else if (isFunction(function[i]))
        {
            if (!stack.count())
            {
                return (ErrorType)2/*ErrorType::NOT_ENOUGH_PARAMETERS*/;
            }

            switch (function[i])
            {
            case TOKEN_SIN:
                stack.push(sin(stack.pop()));
                break;
            case TOKEN_COS:
                stack.push(cos(stack.pop()));
                break;
            }
        }
        else if (function[i] != TOKEN_NOP)
        {
            switch (function[i])
            {
            case TOKEN_VAR:
                stack.push(x);
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (stack.isEmpty())
    {
        result = 0.0f;
    }
    else
    {
        result = stack.pop();
    }
    return (ErrorType)0/*ErrorType::OK*/;
}

ErrorType Function::getFunction(const char* input)
{
    uint8_t inputLen = 0;
    while (input[++inputLen] != '\0');

    uint8_t prevToken;
    uint8_t token = TOKEN_NOP;
    char* endOfNumber;

    StackArray<uint8_t> operators;

    uint8_t outputFIndex = 0;
    uint8_t outputCIndex = 0;

    for (uint8_t i = 0; i < inputLen; i++)
    {
        prevToken = token;
        token = getToken(input[i]);

        // Implicit multiplication
        if ((isValue(prevToken) || prevToken == TOKEN_RBR)
                && (isValue(token) || isFunction(token) || token == TOKEN_LBR))
        {
            operators.push(TOKEN_MUL);
        }

        if (token == TOKEN_UNKNOWN)
        {
            function[outputFIndex++] = token = outputCIndex;
            constants[outputCIndex++] = strtod(input + i, &endOfNumber);
            if (i != endOfNumber - input) i = endOfNumber - input - 1;
        }
        else if (isFunction(token))
        {
            operators.push(token);
        }
        else if (isOperator(token))
        {
            if (operators.count())
            {
                uint8_t nextToken = operators.peek();
                while (
                    isFunction(nextToken) || (
                        isOperator(nextToken) && (
                            getPrecedence(nextToken) > getPrecedence(token) ||
                            getPrecedence(nextToken) == getPrecedence(token) && !getAssociativity(nextToken)
                        )
                    )
                )
                {
                    function[outputFIndex++] = operators.pop();
                    if (operators.count())
                    {
                        nextToken = operators.peek();
                    }
                    else
                    {
                        break;
                    }
                }
            }
            operators.push(token);
        }
        else if (token == TOKEN_LBR)
        {
            operators.push(token);
        }
        else if (token == TOKEN_RBR)
        {
            while (operators.count() && operators.peek() != TOKEN_LBR)
            {
                function[outputFIndex++] = operators.pop();
            }
            if (operators.isEmpty())
            {
                return (ErrorType)1/*ErrorType::PARENTHESIS_MISMATCH*/;
            }
            else
            {
                operators.pop();
            }
        }
        else
        {
            // Probably a special constant.
            function[outputFIndex++] = token;
        }
    }

    while (operators.count())
    {
        function[outputFIndex] = operators.pop();
        if (function[outputFIndex] == TOKEN_LBR) return (ErrorType)1/*ErrorType::PARENTHESIS_MISMATCH*/;
        outputFIndex++;
    }
    function[outputFIndex++] = TOKEN_NOP;

    return (ErrorType)0/*ErrorType::OK*/;
}

uint8_t Function::getToken(const char input)
{
    switch (input)
    {
    case  '+':
        return TOKEN_ADD;
    case  '-':
        return TOKEN_SUB;
    case  '*':
        return TOKEN_MUL;
    case  '/':
        return TOKEN_DIV;
    case  '^':
        return TOKEN_POW;
    case  '%':
        return TOKEN_MOD;
    case  '(':
        return TOKEN_LBR;
    case  ')':
        return TOKEN_RBR;
    case  'x':
        return TOKEN_VAR;
    case '\0':
        return TOKEN_NOP;
    case  's':
        return TOKEN_SIN;
    case  'c':
        return TOKEN_COS;
    default  :
        return TOKEN_UNKNOWN;
    }
}

bool Function::isConstant(const uint8_t input)
{
    return input >= TOKEN_CONSTANT_START && input < TOKEN_VARIABLE_START;
}
bool Function::isValue(const uint8_t input)
{
    return input >= TOKEN_CONSTANT_START && input < TOKEN_OPERATOR_START;
}
bool Function::isOperator(const uint8_t input)
{
    return input >= TOKEN_OPERATOR_START && input < TOKEN_FUNCTION_START;
}
bool Function::isFunction(const uint8_t input)
{
    return input >= TOKEN_FUNCTION_START && input < TOKEN_SPECIAL_START;
}
bool Function::isSpecial(const uint8_t input)
{
    return input >= TOKEN_SPECIAL_START;
}

bool Function::getAssociativity(const uint8_t input)
{
    switch (input)
    {
    case TOKEN_NEG:
    case TOKEN_ADD:
    case TOKEN_SUB:
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_MOD:
        return false;
    case TOKEN_POW:
        return true;
    }
}

uint8_t Function::getPrecedence(const uint8_t input)
{
    switch (input)
    {
    case TOKEN_ADD:
    case TOKEN_SUB:
        return 2;
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_MOD:
        return 3;
    case TOKEN_POW:
        return 4;
    case TOKEN_NEG:
        return 5;
    }
}

uint8_t Function::getParameters(const uint8_t input)
{
    switch (input)
    {
    case TOKEN_ADD:
    case TOKEN_SUB:
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_MOD:
    case TOKEN_POW:
        return 2;
    case TOKEN_NEG:
        return 1;
    }
}
