#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    printk("LiftSense firmware booting...\n");

    while (1) {
        printk("tick\n");
        k_sleep(K_SECONDS(1));
    }
}