#include <stdint.h>

uint8_t value;

extern "C"
{
#include "DCC_Hardware.h"
#include "avr/io.h"

void TIM1_CAPT_vect(void);
void TIM1_COMPA_vect(void);
}

#include <CppUTest/TestHarness.h>

extern int16_t prevtime;
extern uint16_t times[]; //initialize to ones, just because
extern uint8_t time_select;
extern uint8_t check;

TEST_GROUP(DCCHardwareTests)
{

    void setup()
    {
        DCC_Hardware_Initialize();
        time_select = 0;
        ICR1 = 0;
        prevtime = 0;
        times[0] = 58;
        times[1] = 58;
        value = 0xFF;
    }

    void teardown()
    {

    }
};

TEST(DCCHardwareTests, Initialization)
{
    CHECK_EQUAL(30000, OCR1A);
    CHECK_EQUAL(0xFF, value);
}

TEST(DCCHardwareTests, AcceptOne)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    CHECK_EQUAL(0xFF, value); //nothing recorded yet
    ICR1 = 58; //58uS pass
    TIM1_CAPT_vect(); //falling edge, half the pulse is now seen
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 58; //58uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    ICR1 += 58; //58uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    CHECK_EQUAL(1, value); //now it should be recorded.
}

TEST(DCCHardwareTests, RejectOneTooShort)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    CHECK_EQUAL(0xFF, value); //nothing recorded yet
    ICR1 = 50; //50uS fail
    TIM1_CAPT_vect(); //falling edge, half the pulse is now seen
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 50; //50uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    CHECK_EQUAL(0xFF, value); //now it should be recorded.
}

TEST(DCCHardwareTests, RejectOneBadDutyCycle)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    CHECK_EQUAL(0xFF, value); //nothing recorded yet
    ICR1 = 64; //64uS pass
    TIM1_CAPT_vect(); //falling edge, half the pulse is now seen
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 52; //52uS more—within tolerance, but more than 6uS off from first value, so shoudl reject
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    CHECK_EQUAL(0xFF, value); //now it should be recorded.
}

TEST(DCCHardwareTests, AcceptOneReversePolarity)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    ICR1 = 100; //100uS pass, this is the second half of a zero, but reverse polarity
    TIM1_CAPT_vect(); //falling edge
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 58; //58uS more, this is the falling edge, but first half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(0xFF, value); //nothing to record yet
    ICR1 += 58; //58uS more, this is the rising edge, but second half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(1, value); //now it should be recorded.
    //now send another 1 and make sure
    value = 0xFF;
    ICR1 += 58; //58uS more, this is the falling edge, but first half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(0xFF, value); //nothing to record yet
    ICR1 += 58; //58uS more, this is the rising edge, but second half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(1, value); //now it should be recorded.
}

/// CHECK ZEROs

TEST(DCCHardwareTests, AcceptZero)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    CHECK_EQUAL(0xFF, value); //nothing recorded yet
    ICR1 = 100; //100uS pass
    TIM1_CAPT_vect(); //falling edge, half the pulse is now seen
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 100; //100uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    ICR1 += 100; //100uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;    ICR1 += 100; //100uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    CHECK_EQUAL(0, value); //now it should be recorded.
}

TEST(DCCHardwareTests, RejectZeroTooShort)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    CHECK_EQUAL(0xFF, value); //nothing recorded yet
    ICR1 = 89; //89uS
    TIM1_CAPT_vect(); //falling edge, half the pulse is now seen
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 89; //89uS more
    TIM1_CAPT_vect(); //rising edge at time 116uS;
    CHECK_EQUAL(0xFF, value); //now it should be recorded.
}

TEST(DCCHardwareTests, AcceptZeroReversePolarity)
{
    TIM1_CAPT_vect(); //rising edge at time 0;
    ICR1 = 58; //58uS pass, this is the second half of a one, but reverse polarity
    TIM1_CAPT_vect(); //falling edge
    CHECK_EQUAL(0xFF, value); //nothing should be recorded yet
    ICR1 += 100; //100uS more, this is the falling edge, but first half of a zero
    TIM1_CAPT_vect();
    CHECK_EQUAL(0xFF, value); //nothing to record yet
    ICR1 += 100; //58uS more, this is the rising edge, but second half of a zero
    TIM1_CAPT_vect();
    CHECK_EQUAL(0, value); //now it should be recorded.
    //now send another 1 and make sure
    value = 0xFF;
    ICR1 += 58; //58uS more, this is the falling edge, but first half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(0xFF, value); //nothing to record yet
    ICR1 += 58; //58uS more, this is the rising edge, but second half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(1, value); //now it should be recorded.
}

//TODO test case od ICR1 rollover!

//TEST DC conversion mode

IGNORE_TEST(DCCHardwareTests, EnterDCMode)
{
    //first, pass a one
    ICR1 += 58; //58uS more, this is the falling edge, but first half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(0xFF, value); //nothing to record yet
    ICR1 += 58; //58uS more, this is the rising edge, but second half of a one
    TIM1_CAPT_vect();
    CHECK_EQUAL(1, value); //now it should be recorded.

    //now, make the timer counter hit ICR1 + 30000
    TIM1_COMPA_vect();
}