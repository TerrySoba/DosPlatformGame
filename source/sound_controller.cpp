#include "sound_controller.h"

SoundController::SoundController()
{
    m_sbFound = m_sb.soundBlasterFound();
    m_deathSound = m_sb.loadVocFile("DEATH.VOC");
    m_walkSound = m_sb.loadVocFile("STEPS.VOC");
}

void SoundController::playJumpSound()
{

}

void SoundController::playWalkSound()
{
    if (m_sbFound)
    {
        m_sb.singlePlay(m_walkSound);
    }
}

void SoundController::playDeathSound()
{
    if (m_sbFound)
    {
        m_sb.singlePlay(m_deathSound);
    }
}
