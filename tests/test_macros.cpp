#include <pans/macros.h>
#include <iostream>
#include <type_traits>
#include <limits>



static_assert(sizeof(u8) == 1 && std::is_unsigned_v<u8>, "u8 must be an unsigned 8-bit integer type");
static_assert(sizeof(s8) == 1 && std::is_signed_v<s8>, "s8 must be a signed 8-bit integer type");
static_assert(sizeof(u16) == 2 && std::is_unsigned_v<u16>, "u16 must be an unsigned 16-bit integer type");
static_assert(sizeof(s16) == 2 && std::is_signed_v<s16>, "s16 must be a signed 16-bit integer type");
static_assert(sizeof(u32) == 4 && std::is_unsigned_v<u32>, "u32 must be an unsigned 32-bit integer type");
static_assert(sizeof(s32) == 4 && std::is_signed_v<s32>, "s32 must be a signed 32-bit integer type");
static_assert(sizeof(u64) == 8 && std::is_unsigned_v<u64>, "u64 must be an unsigned 64-bit integer type");
static_assert(sizeof(s64) == 8 && std::is_signed_v<s64>, "s64 must be a signed 64-bit integer type");


static_assert(INVALID8 == MAX_U8, "INVALID8 must be equal to MAX_U8");
static_assert(INVALID16 == MAX_U16, "INVALID16 must be equal to MAX_U16");
static_assert(INVALID32 == MAX_U32, "INVALID32 must be equal to MAX_U32");
static_assert(INVALID64 == MAX_U64, "INVALID64 must be equal to MAX_U64");

static_assert(INVALID8 == std::numeric_limits<u8>::max(), "INVALID8 must be equal to the maximum value of u8");
static_assert(INVALID16 == std::numeric_limits<u16>::max(), "INVALID16 must be equal to the maximum value of u16");
static_assert(INVALID32 == std::numeric_limits<u32>::max(), "INVALID32 must be equal to the maximum value of u32");
static_assert(INVALID64 == std::numeric_limits<u64>::max(), "INVALID64 must be equal to the maximum value of u64");



void test_types_and_constants() {
    ASSERT_NOEFFECT(sizeof(u8) == 1 && std::is_unsigned_v<u8>);
    ASSERT_NOEFFECT(sizeof(s8) == 1 && std::is_signed_v<s8>);
    ASSERT_NOEFFECT(sizeof(u16) == 2 && std::is_unsigned_v<u16>);
    ASSERT_NOEFFECT(sizeof(s16) == 2 && std::is_signed_v<s16>);
    ASSERT_NOEFFECT(sizeof(u32) == 4 && std::is_unsigned_v<u32>);
    ASSERT_NOEFFECT(sizeof(s32) == 4 && std::is_signed_v<s32>);
    ASSERT_NOEFFECT(sizeof(u64) == 8 && std::is_unsigned_v<u64>);
    ASSERT_NOEFFECT(sizeof(s64) == 8 && std::is_signed_v<s64>);

    ASSERT_NOEFFECT(INVALID8 == MAX_U8);
    ASSERT_NOEFFECT(INVALID16 == MAX_U16);
    ASSERT_NOEFFECT(INVALID32 == MAX_U32);
    ASSERT_NOEFFECT(INVALID64 == MAX_U64);

    ASSERT_RETNONE(INVALID8 == std::numeric_limits<u8>::max());
    ASSERT_RETNONE(INVALID16 == std::numeric_limits<u16>::max());
    ASSERT_RETNONE(INVALID32 == std::numeric_limits<u32>::max());
    ASSERT_RETNONE(INVALID64 == std::numeric_limits<u64>::max());

    ASSERT_RETNONE2(INVALID8 == std::numeric_limits<u8>::max(), "INVALID8 must be equal to the maximum value of u8");
    ASSERT_RETNONE2(INVALID16 == std::numeric_limits<u16>::max(), "INVALID16 must be equal to the maximum value of u16");
    ASSERT_RETNONE2(INVALID32 == std::numeric_limits<u32>::max(), "INVALID32 must be equal to the maximum value of u32");
    ASSERT_RETNONE2(INVALID64 == std::numeric_limits<u64>::max(), "INVALID64 must be equal to the maximum value of u64");
}

[[nodiscard]] int test_assert_retval()
{
    ASSERT_RETVAL(false, -1);
    return 0;
}


[[nodiscard]] int test_assert_retval2()
{
    ASSERT_RETVAL2(false, -1, "This is a test message for ASSERT_RETVAL2");
    return 0;
}


void test_assert_macros()
{
    int never_reached = 5;  //设定一个不可能达到的值
    int i = 0;

    for(i = 0; i < 10; i++)
    {
        ASSERT_CONTINUE(i != never_reached);
    }

    std::cout<<"1--------------------------------------------------:"<<i<<std::endl;

    for ( i = 0; i < 10; i++)
    {
        ASSERT_CONTINUE2(i != never_reached, "i should not be never_reached");
    }
    
    std::cout<<"2--------------------------------------------------:"<<i<<std::endl;

    for (i = 0; i < 10; i++)
    {
        ASSERT_BREAK(i != never_reached);
    }

    std::cout<<"3--------------------------------------------------:"<<i<<std::endl;

    for (i = 0; i < 10; i++)
    {
        ASSERT_BREAK2(i != never_reached, "i should not be never_reached");
    }

    std::cout<<"4--------------------------------------------------:"<<i<<std::endl;


    for (i = 0; i < 10; i++)
    {
        ASSERT_NOEFFECT(i != never_reached);
    }

    std::cout<<"5--------------------------------------------------:"<<i<<std::endl;

    for (i = 0; i < 10; i++)
    {
        ASSERT_NOEFFECT2(i != never_reached, "i should not be never_reached");
    }

    std::cout<<"6--------------------------------------------------:"<<i<<std::endl;
    
}



int main(int argc, char * const argv[])
{
#ifdef PANS_DEBUG
    std::cout<<"PANS_DEBUG is defined"<<std::endl;
#endif

#ifdef NDEBUG
    std::cout<<"NDEBUG is defined"<<std::endl;
#endif

    test_types_and_constants();
    test_assert_macros();

    auto retval1 = test_assert_retval();
    std::cout<<"test_assert_retval returned: "<<retval1<<std::endl;

    auto retval2 = test_assert_retval2();
    std::cout<<"test_assert_retval2 returned: "<<retval2<<std::endl;

    return EXIT_SUCCESS;
}
