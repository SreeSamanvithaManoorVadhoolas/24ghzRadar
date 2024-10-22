/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "fft_application.h"

// ISR declarations
CY_ISR_PROTO(isr_button_RisingEdge_Interrupt);
CY_ISR_PROTO(isr_UART);

// Enum for state machine states
typedef enum
{
    IDLE_STATE,
    SAMPLING_STATE,
    UART_TRANSFER_STATE

}STATE_t;

// State machine function declaration
void stateMachine();

// Variable declarations
volatile uint8 button_pressed = 0; // Push button status
uint8_t sReceived = 0; // status of s character recieved 
uint8_t oReceived = 0; // status of o character recieved
char RxMatlab; // Character recieved from MATLAB
uint8_t count = 0; // Transfer count
uint16_t ADC_Array[1024] = {0}; // Array to store ADC data
STATE_t state = IDLE_STATE; // State machine state initialization
int32 FFT_SamplesArray[2 * 1024] = {0}; // Array to store FFT samples

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    isr_button_StartEx(isr_button_RisingEdge_Interrupt);
    isr_UART_StartEx(isr_UART);
    
    //Hardware Init
    UART_Start();
    WaveDAC_Start();
    ADC_Start();
    ADC_StartConvert();
    
    for(;;)
    {
        stateMachine();
    }
}

void stateMachine()
{
    switch(state)
    {
        case IDLE_STATE:
            count = 0;
            // Handle LED states
            Red_LED_Write(1);
            Yellow_LED_Write(0);     
            Green_LED_Write(0);
            
            if(button_pressed == 1)
            {
                // Setting state to SAMPLING_STATE when button is pressed
                state = SAMPLING_STATE;
                button_pressed = 0;               
            }
        break;
            
        case SAMPLING_STATE:
            // Handle LED states
            Red_LED_Write(0);
            Yellow_LED_Write(1);     
            Green_LED_Write(0);
            // ADC conversion and storing data in array
            for(int i = 0; i < 1024; i++)
            {
                ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);                  
                ADC_Array[i] = (uint16_t)ADC_GetResult32();
            }
            // Setting state to UART_TRANSFER_STATE if 's' received from MATLAB
            if(sReceived)
            {             
                state = UART_TRANSFER_STATE;
                sReceived = 0;
            }
        break;
            
        case UART_TRANSFER_STATE:
            // Handle LED states
            Red_LED_Write(0);
            Yellow_LED_Write(0);     
            Green_LED_Write(1);
            // Transfer ADC data to MATLAB
            for(int i = 0; i < 1024; i++)
            {
                UART_PutChar(ADC_Array[i]);
                UART_PutChar(ADC_Array[i]>>8);
            }
            // FFT calculation of ADC data
            fft_app(ADC_Array,FFT_SamplesArray,1024);
            
            // Transfer FFT data to MATLAB
            for(int32 idx = 0; idx < 2048; idx++)
            {
                UART_PutChar(FFT_SamplesArray[idx]);
                UART_PutChar(FFT_SamplesArray[idx]>>8);
                UART_PutChar(FFT_SamplesArray[idx]>>16);
                UART_PutChar(FFT_SamplesArray[idx]>>24);
            }
            // Increment count
            count++;                
            
            // Move to next transfer if count < 10
            if((count < 10) && oReceived)
            {
                state = SAMPLING_STATE;
                oReceived = 0;
            }
            // Setting state to IDLE_STATE when count is 10
            else if((count == 10) && oReceived)
            {
                // Handle LED states
                Red_LED_Write(0);
                Yellow_LED_Write(0);     
                Green_LED_Write(0);
                state = IDLE_STATE;  
                oReceived = 0;
            }
            else // Stay in same state
            {
                state = UART_TRANSFER_STATE;  
                oReceived = 0; 
            }
        break;
            
        default:
        break;
    }   
}

    
CY_ISR(isr_button_RisingEdge_Interrupt)
{
    // Update button_pressed variable based on push button
    Push_button_ClearInterrupt();
    
    if(button_pressed == 0)
    {
        button_pressed = 1;
    }
}

CY_ISR(isr_UART)
{
    // Set character flags based on character received from MATLAB
    RxMatlab = UART_GetChar();
    if(RxMatlab == 's')
    {
        sReceived = 1;
    }
    if(RxMatlab == 'o')
    {
        oReceived = 1;
    }
}
/* [] END OF FILE */
