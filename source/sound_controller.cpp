#include "sound_controller.h"

SoundController::SoundController() :
    m_soundPriority(0)
{

    m_sbFound = m_sb.soundBlasterFound();
    m_jumpSound = m_sb.loadVocFile("JUMP.VOC");
    m_deathSound = m_sb.loadVocFile("DEATH.VOC");
    m_walkSound = m_sb.loadVocFile("STEPS.VOC");
    m_standSound = m_sb.loadVocFile("STAND.VOC");
    m_guffinSound = m_sb.loadVocFile("GUFFIN.VOC");
    m_jetpackSound = m_sb.loadVocFile("JETPACK.VOC");
    m_switchSound = m_sb.loadVocFile("SWITCH2.VOC");
}

SoundController::~SoundController()
{
    m_sb.freeSample(m_jumpSound);
    m_sb.freeSample(m_deathSound);
    m_sb.freeSample(m_walkSound);
    m_sb.freeSample(m_standSound);
    m_sb.freeSample(m_guffinSound);
    m_sb.freeSample(m_jetpackSound);
    m_sb.freeSample(m_switchSound);
}



#define priority(x) ((x) > (m_soundPriority) || !m_sb.isPlaying())


void SoundController::playJumpSound()
{
    if (m_sbFound)
    {
        if (priority(3))
        {
            m_soundPriority = 3;
            m_sb.singlePlay(m_jumpSound);
        }
    }
}

void SoundController::playWalkSound()
{
    if (m_sbFound)
    {
        if (priority(1))
        {
            m_soundPriority = 1;
            m_sb.singlePlay(m_walkSound);
        }
    }
}

void SoundController::playStandSound()
{
    if (m_sbFound)
    {
        if (priority(2))
        {
            m_soundPriority = 2;
            m_sb.singlePlay(m_standSound);
        }
    }
}

void SoundController::playSwitchSound()
{
    if (m_sbFound)
    {
        if (priority(5))
        {
            m_soundPriority = 5;
            m_sb.singlePlay(m_switchSound);
        }
    }
}


void SoundController::playGuffinSound()
{
    if (m_sbFound)
    {
        m_soundPriority = 4;
        m_sb.singlePlay(m_guffinSound);
    }
}

void SoundController::playJetpackSound()
{
    if (m_sbFound)
    {
        m_soundPriority = 4;
        m_sb.singlePlay(m_jetpackSound);
    }
}

void SoundController::playDeathSound()
{
    if (m_sbFound)
    {
        m_soundPriority = 4;
        m_sb.singlePlay(m_deathSound);
    }
}
