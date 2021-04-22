#include "mbed.h"


InterruptIn button(USER_BUTTON);
DigitalOut led(LED1);
Timeout  press_threhold;
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void button_release_detecting()
{
    button.enable_irq();
}



void button_pressed()
{
    button.disable_irq();
    press_threhold.attach(&button_release_detecting, 1.0);
}

void button_released()
{
    led = !led;
    printf("success\n");
}


// main() runs in its own thread in the OS
int main()
{
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    printf("Starting in context %p\r\n", ThisThread::get_id());
    button.fall(&button_pressed);
    button.rise(queue.event(&button_released));
    while (1);
}