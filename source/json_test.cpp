#include "unit_test.h"
#include "json.h"
#include "exception.h"

TEST(JsonTest1)
{
    Filename filename("..\\images\\guy.jsn");
    Json json(filename);
    JsonValue value = json.getRoot();
    ASSERT_FALSE(value.isInvalid());
    ASSERT_FALSE(value.isFalse());  
    ASSERT_FALSE(value.isTrue());   
    ASSERT_FALSE(value.isBool());   
    ASSERT_FALSE(value.isNull());   
    ASSERT_FALSE(value.isNumber()); 
    ASSERT_FALSE(value.isString()); 
    ASSERT_FALSE(value.isArray());  
    ASSERT_TRUE(value.isObject());
    ASSERT_FALSE(value.isRaw());  
}

TEST(JsonTest2)
{
    Json json("12.5");
    JsonValue value = json.getRoot();
    ASSERT_FALSE(value.isInvalid());
    ASSERT_FALSE(value.isFalse());  
    ASSERT_FALSE(value.isTrue());   
    ASSERT_FALSE(value.isBool());   
    ASSERT_FALSE(value.isNull());   
    ASSERT_TRUE(value.isNumber());
    ASSERT_FALSE(value.isString()); 
    ASSERT_FALSE(value.isArray());  
    ASSERT_FALSE(value.isObject());
    ASSERT_FALSE(value.isRaw());

    ASSERT_TRUE(value.toDouble() == 12.5);
}

TEST(JsonTest3)
{
    Json json("\"Hello\"");
    JsonValue value = json.getRoot();
    ASSERT_FALSE(value.isInvalid());
    ASSERT_FALSE(value.isFalse());  
    ASSERT_FALSE(value.isTrue());   
    ASSERT_FALSE(value.isBool());   
    ASSERT_FALSE(value.isNull());   
    ASSERT_FALSE(value.isNumber());
    ASSERT_TRUE(value.isString()); 
    ASSERT_FALSE(value.isArray());  
    ASSERT_FALSE(value.isObject());
    ASSERT_FALSE(value.isRaw());

    ASSERT_TRUE(value.toString() == "Hello");
}

TEST(JsonTest4)
{
    Json json("[1,2,3]");
    JsonValue value = json.getRoot();
    ASSERT_FALSE(value.isInvalid());
    ASSERT_FALSE(value.isFalse());  
    ASSERT_FALSE(value.isTrue());   
    ASSERT_FALSE(value.isBool());   
    ASSERT_FALSE(value.isNull());   
    ASSERT_FALSE(value.isNumber());
    ASSERT_FALSE(value.isString()); 
    ASSERT_TRUE(value.isArray());  
    ASSERT_FALSE(value.isObject());
    ASSERT_FALSE(value.isRaw());  

    ASSERT_TRUE(value.size() == 3);
    ASSERT_THROW(value.at(value.size()), Exception);
}

TEST(JsonTest5)
{
    Json json("{\"test\" : 1, \"lala\" : \"value\"}");
    JsonValue value = json.getRoot();
    ASSERT_FALSE(value.isInvalid());
    ASSERT_FALSE(value.isFalse());  
    ASSERT_FALSE(value.isTrue());   
    ASSERT_FALSE(value.isBool());   
    ASSERT_FALSE(value.isNull());   
    ASSERT_FALSE(value.isNumber());
    ASSERT_FALSE(value.isString()); 
    ASSERT_FALSE(value.isArray());  
    ASSERT_TRUE(value.isObject());
    ASSERT_FALSE(value.isRaw());  

    ASSERT_TRUE(value.size() == 2);
    ASSERT_THROW(value.at(100), Exception);
    
    ASSERT_THROW(value.at("Test"), Exception);

    ASSERT_TRUE(value.at("lala").toString() == "value");
    ASSERT_TRUE(value.at("test").toInt() == 1);
    ASSERT_TRUE(value.at("test").toDouble() == 1.0);

    ASSERT_THROW(value.at("test").toString(), Exception);

    ASSERT_TRUE(value.at(0).keyString() == "test");
    ASSERT_TRUE(value.at(1).keyString() == "lala");
}
