#include "rad_player.h"

#include "exception.h"
#include "tiny_string.h"
#include "safe_read.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern "C"
{
    extern void radPlayMusic();
    extern void radEndPlayer();
	extern uint16_t radInitPlayer(const uint8_t far* songData);
}

/**
 * Returns addresses that are aligned to 16byte
 */
static void* allignedMalloc(int size, void** origBlock = NULL)
{
	char* ptr = (char*)malloc(size + 16);
	if (origBlock) *origBlock = (void*)ptr;
	ptr += 16ul - ((intptr_t)ptr & 0xf);
	return (void*)ptr;
}

void RadPlayer::radLoadModuleInternal(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        THROW_EXCEPTION("Could not open file:", filename);
    }

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (m_songDataSize < len)
	{
		if (m_songDataOrigBlock)
		{
			free(m_songDataOrigBlock);
		}
		m_songData = (uint8_t*)allignedMalloc(len, &m_songDataOrigBlock);
		m_songDataSize = len;
	}
	
	uint8_t* buf = (uint8_t*)m_songData;
	while (!feof(fp))
	{
		int bytes = safeRead(buf, 1, 1000, fp);
		buf += bytes;
	}

	if (!radInitPlayer((uint8_t*)m_songData))
	{
		printStr("Could not initialize RAD player. Maybe file is corrupted.\r\n");
		fclose(fp);
	}

	fclose(fp);
}

static void timerFunction()
{
	radPlayMusic();
}

RadPlayer::RadPlayer() :
	m_songDataOrigBlock(0),
	m_songData(0),
	m_songDataSize(0)
{
}

void RadPlayer::playModule(const char* filename)
{
	stopModule();
	radLoadModuleInternal(filename);
	if (!m_songData)
	{
		THROW_EXCEPTION("Error loading rad module: ", filename);
	}
	m_timer = new DosTimer(timerFunction, 50);
}

void RadPlayer::stopModule()
{
	m_timer.reset();
	radEndPlayer();
}

RadPlayer::~RadPlayer()
{
	stopModule();
	free(m_songDataOrigBlock);
}
