typedef unsigned char u8;
typedef unsigned short u16;

struct ExecParamBlock
{
    u16 envSeg;
    u16 cmdTailOff;
    u16 cmdTailSeg;
    u16 fcb1Off;
    u16 fcb1Seg;
    u16 fcb2Off;
    u16 fcb2Seg;
};

u16 dosGetPspSegment(void);
#pragma aux dosGetPspSegment = \
    "mov ah, 0x62" \
    "int 0x21" \
    value   [bx] \
    modify  [ax];

u16 dosExec(const char* program, const struct ExecParamBlock* block);
#pragma aux dosExec = \
    "push ds"         \
    "pop es"          \
    "mov ax, 0x4B00" \
    "int 0x21"       \
    "sbb ax, ax"     \
    "neg ax"         \
    parm    [dx] [bx] \
    value   [ax] \
    modify  [ax es];

u8 dosGetExitCode(void);
#pragma aux dosGetExitCode = \
        "mov ah, 0x4D" \
        "int 0x21"     \
        value   [al] \
        modify  [ax];

int spawnWait(const char* program)
{
    struct ExecParamBlock block;
    u16 pspSeg = dosGetPspSegment();

    block.envSeg = 0;
    block.cmdTailOff = 0x0080;
    block.cmdTailSeg = pspSeg;
    block.fcb1Off = 0;
    block.fcb1Seg = 0;
    block.fcb2Off = 0;
    block.fcb2Seg = 0;

    if (dosExec(program, &block) != 0)
    {
        return -1;
    }

    return (int)dosGetExitCode();
}

void printCh(char ch);
#pragma aux printCh = \
        "mov ah, 02h"   \
        "int 0x21"      \
        parm    [dl]    \
        modify  [ax];

void printStr(const char* str)
{
    while (str && *str)
    {
        printCh(*str);
        ++str;
    }
}

// void printInteger(int value)
// {
//     char buffer[2];
//     buffer[1] = '$';
//     int pos = 0;
//     buffer[pos] = '0' + value;
//     print(buffer);
// }


int main(int argc, char* argv[])
{
    int ret;

    (void)argc;
    (void)argv;

    ret = spawnWait("title.exe");
    if (ret == 1)
    {
        printStr("Launching game, please wait...\r\n");
        ret = spawnWait("game2.exe");
        return ret;
    }

    return 0;
}
