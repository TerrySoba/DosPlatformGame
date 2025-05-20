#include "ini_file.h"

#include "exception.h"
#include "unit_test.h"


TEST(IniFileTest_exception)
{
    ASSERT_THROW(
        IniFile ini("unknown_file.ini"); ini.getString("name");,
        Exception
    );
}

TEST(IniFileTestUnix)
{
    IniFile ini(TEST_DATA_DIR "test_lf.ini");
    ASSERT_TRUE(ini.getString("name") == "MyName");
    ASSERT_TRUE(ini.getString("age") == "43");
    ASSERT_TRUE(ini.getString("mail") == "Some@some.org");
    ASSERT_TRUE(ini.getString("id") == "123456");


    ASSERT_TRUE(ini.getBool("bool1").value() == true);
    ASSERT_TRUE(ini.getBool("bool2").value() == true);
    ASSERT_TRUE(ini.getBool("bool3").value() == false);
    ASSERT_TRUE(ini.getBool("bool4").value() == false);
    ASSERT_TRUE(ini.getBool("bool5").value() == true);
    ASSERT_TRUE(ini.getBool("bool6").value() == false);
    ASSERT_TRUE(!ini.getBool("bool7").has_value());
    ASSERT_TRUE(!ini.getBool("bool8").has_value());
 
    
    ASSERT_TRUE(ini.getInt("int1").value() == 123);
    ASSERT_TRUE(ini.getInt("int2").value() == 0);
    ASSERT_TRUE(ini.getInt("int3").value() == -123);
}

TEST(IniFileTestWindows)
{
    IniFile ini(TEST_DATA_DIR "test_crlf.ini");
    ASSERT_TRUE(ini.getString("name") == "MyName");
    ASSERT_TRUE(ini.getString("age") == "43");
    ASSERT_TRUE(ini.getString("mail") == "Some@some.org");
    ASSERT_TRUE(ini.getString("id") == "123456");
}

TEST(IniFileTestWriteIniFile)
{
    {
        IniFile ini(TEST_DATA_DIR "empty.ini");
        ini.setString("name", "Some Guy");
        ini.setString("age", "123");
        ini.save();
    }
    {
        IniFile ini(TEST_DATA_DIR "empty.ini");
        ASSERT_TRUE(ini.getString("name") == "Some Guy");
        ASSERT_TRUE(ini.getString("age") == "123");
    }
}
