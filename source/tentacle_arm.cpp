#include "tentacle_arm.h"
#include "physics.h"
#include "math.h"

TentacleArm::TentacleArm(int posX, int posY, tnd::shared_ptr<Animation> animation)
{
    m_posX = posX;
    m_posY = posY;
    m_animation = animation;
    m_armExtension = 0;

    m_segments.push_back(Rectangle(m_posX, m_posY, PIXEL_TO_SUBPIXEL(m_animation->width()), PIXEL_TO_SUBPIXEL(m_animation->height())));
    m_segments.push_back(Rectangle(m_posX, m_posY, PIXEL_TO_SUBPIXEL(m_animation->width()), PIXEL_TO_SUBPIXEL(m_animation->height())));
    m_segments.push_back(Rectangle(m_posX, m_posY, PIXEL_TO_SUBPIXEL(m_animation->width()), PIXEL_TO_SUBPIXEL(m_animation->height())));
    m_segments.push_back(Rectangle(m_posX, m_posY, PIXEL_TO_SUBPIXEL(m_animation->width()), PIXEL_TO_SUBPIXEL(m_animation->height())));
}

TentacleArm::~TentacleArm()
{

}

template <class T>
T myAbs(T x) {
    return ((x) < 0 ? -(x) : (x));
}


const int MAX_ARM_EXTENSION = 48;
const int MIN_ARM_EXTENSION = 0;

const int16_t WIGGLE_TABLE[] = {0,1,2,3,3,3,2,2,0,-2, -3, -3, -3, -2, -1};
const int WIGGLE_TABLE_SIZE = sizeof(WIGGLE_TABLE) / sizeof(WIGGLE_TABLE[0]);


void TentacleArm::act(int actorPosX, int actorPosY)
{
    ++m_wiggleFrame;

    if (myAbs(m_posX - actorPosX) < PIXEL_TO_SUBPIXEL(70))
    {
        if (m_armExtension < MAX_ARM_EXTENSION)
        {
            m_armExtension += 1;
        }
    } else {
        if (m_armExtension > MIN_ARM_EXTENSION)
        {
            m_armExtension -= 1;
        }
    }

    if (m_armExtension == 0)
    {
        for (int i = 0; i < m_segments.size(); ++i)
        {
            m_segments[i].x = m_posX;
            m_segments[i].y = m_posY;
        }
    }
    else
    {
        for (int i = 0; i < m_segments.size(); ++i)
        {
            m_segments[i].x = m_posX + PIXEL_TO_SUBPIXEL(WIGGLE_TABLE[((m_wiggleFrame >> 3) + i) % WIGGLE_TABLE_SIZE]);
            m_segments[i].y = m_posY - PIXEL_TO_SUBPIXEL((m_armExtension >> 2) * i);
        }
    }
    
}

tnd::vector<Rectangle> TentacleArm::getSegments()
{
    return m_segments;
}
