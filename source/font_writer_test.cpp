#include "unit_test.h"
#include "exception.h"
#include "font.h"
#include "font_writer.h"

#include "test_data.h"

TEST(FontWriterTest1)
{
    Font font(TEST_DATA_DIR "ubuntu10.stf");
    FontWriter writer(&font);

    writer.setText("Hello World");
    ASSERT_EQ_INT(writer.width(), 57);
    ASSERT_EQ_INT(writer.height(), 12);

    writer.setText("Short");
    ASSERT_EQ_INT(writer.width(), 26);
    ASSERT_EQ_INT(writer.height(), 12);
}
