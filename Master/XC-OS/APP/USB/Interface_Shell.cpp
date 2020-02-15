#include "USB_Private.h"
#include "GUI/DisplayPrivate.h"

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
static int Shell_InterfaceReader(char * data)
{
    // Wrapper for Serial.read() method
    if (Serial.available())
    {
        *data = Serial.read();
        return 1;
    }
    return 0;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype
 * void my_writer_function(char data)
 */
static void Shell_InterfaceWriter(char data)
{
    // Wrapper for Serial.write() method
    Serial.write(data);
}

static int Shell_Exit(int argc, char** argv)
{
    page.PagePop();
    return SHELL_RET_SUCCESS;
}

void Shell_Init()
{
    Serial.begin(115200);
    Serial.setTimeout(10);
    
    shell_set_prompt_msg(FS_GetPromptMsg());
    shell_init(Shell_InterfaceReader, Shell_InterfaceWriter, NULL);
    
    shell_register(Shell_Exit, "exit");
    
    ShellReg_FS();
    ShellReg_HW();
}
