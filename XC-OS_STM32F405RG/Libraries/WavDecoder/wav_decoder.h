
#ifndef __WAVDECODER_H__
#define __WAVDECODER_H__

#include "stdint.h"

#define WAV_ERRCODE_CannotFindDataFlag		0xf0
#define WAV_ERRCODE_EndOfFile				0xf1

#define WAV_DataFlagOffset_MAX				1024

typedef uint32_t HWAVEFILE;			/* ��ʾ wave �ļ�ʵ��������� */

typedef uint8_t(*WAV_Stream_Interface_TypeDef)(HWAVEFILE hFile, uint8_t reqBytes, uint8_t **buffer);

#pragma pack (1)
typedef struct
{
	uint8_t  FileFlag[4];			/* �ļ���ʶ��һ���� �ַ��� "RIFF" (0x46464952) */
	uint32_t FileLength;		/* �ļ����ȣ���ֵ���� 8 ��Ϊ�ļ���С��8 byte����Ϊ���ļ����Ȳ�����ǰ2�������ǰ����������һ�� 8 byte�� */
	uint8_t  FileType[4];			/* �ļ���ʽ��һ���� �ַ��� "WAVE" (0x45564157)*/
	uint8_t  ChunkFlag[4];			/* ��ʽ���ʶ��һ���� �ַ��� "fmt" (0xXX746D66) */
	uint32_t ChuckLength;		/* ��ʽ�鳤�ȣ����ݱ����ʽ��һ���� 16��18��20��40 */
	uint16_t EncodeType;		/* �����ʽ������ļ�ʹ�� PCM ���룬��Ϊ 1 */
	uint16_t ChannelCnt;		/* ����������������Ϊ 1��˫����Ϊ 2 */
	uint32_t SampleFreq;		/* ����Ƶ�ʣ�һ��Ϊ 11025��22050��44100��48000 */
	uint32_t BytePerSecond;		/* �����ʣ������� * ������ * ����λ�� / 8 */
	uint16_t BytePerFrame;		/* ����֡��С�������� * ����λ�� / 8 */
	uint16_t SampleBits;		/* ����λ����һ���� 4��8��12��16��24��32 */
} WAV_Header_TypeDef;

typedef struct
{
	int32_t LeftVal;
	int32_t RightVal;
} WAV_DataChunk_TypeDef;

#pragma pack ()

typedef struct
{
	HWAVEFILE hWave;
	uint8_t IsEnd;
	uint32_t DataSize;
	uint32_t DataPosition;
	WAV_Header_TypeDef Header;
	WAV_DataChunk_TypeDef CurrentData;
	WAV_Stream_Interface_TypeDef StreamInterface;
} WAV_TypeDef;

HWAVEFILE	Wav_StructInit(WAV_TypeDef *pWav, WAV_Stream_Interface_TypeDef streamProc);
int			Wav_Open(WAV_TypeDef *pWav);
int			Wav_Next_16Bit2Channel(WAV_TypeDef *pWav);

#endif


