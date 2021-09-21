#include "unit_test.h"
#include "csv_reader.h"
#include "rectangle.h"
#include "detect_lines.h"

TEST(CsvReaderTest)
{
    CsvReader<int> reader("source\\test.csv");
    ASSERT_TRUE(reader.width() == 3);
    ASSERT_TRUE(reader.height() == 4);

    ASSERT_TRUE(reader.get(0,0) == 1);
    ASSERT_TRUE(reader.get(2,0) == 3);
    ASSERT_TRUE(reader.get(2,1) == 6);
    ASSERT_TRUE(reader.get(2,3) == 12);
}

TEST(CsvReaderTest2)
{
    CsvReader<int> reader("source\\test2.csv");
    ASSERT_TRUE(reader.width() == 58);
    ASSERT_TRUE(reader.height() == 47);

    ASSERT_TRUE(reader.get(10,38) == 622);
}

TEST(CsvReaderTest3)
{
    CsvReader<int> reader("testdata\\level1.csv");
    ASSERT_TRUE(reader.width() == 20);
    ASSERT_TRUE(reader.height() == 12);

    ASSERT_TRUE(reader.get(0,11) == 122);
    ASSERT_TRUE(reader.get(19,11) == 60);
}

TEST(CsvReaderTest4)
{
    CsvReader<int> reader("testdata\\lines.csv");
    ASSERT_TRUE(reader.width() == 4);
    ASSERT_TRUE(reader.height() == 3);
}


TEST(LineDetectorTest)
{
    CsvReader<uint8_t> reader("testdata\\lines.csv");

    tnd::vector<Rectangle> horizontalLines = detectLines(reader, HORIZONTAL, 1);
    ASSERT_TRUE(horizontalLines.size() == 3);
    ASSERT_TRUE(horizontalLines[0] == Rectangle(0, 1, 2, 1));
    ASSERT_TRUE(horizontalLines[1] == Rectangle(3, 1, 1, 1));
    ASSERT_TRUE(horizontalLines[2] == Rectangle(3, 2, 1, 1));


    tnd::vector<Rectangle> verticalLines = detectLines(reader, VERTICAL, 1);
    ASSERT_TRUE(verticalLines.size() == 3);
    ASSERT_TRUE(verticalLines[0] == Rectangle(0, 1, 1, 1));
    ASSERT_TRUE(verticalLines[1] == Rectangle(1, 1, 1, 1));
    ASSERT_TRUE(verticalLines[2] == Rectangle(3, 1, 1, 2));
}
