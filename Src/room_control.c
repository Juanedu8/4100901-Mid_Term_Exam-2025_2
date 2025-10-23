#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM

// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variable de estado global
room_state_t current_state = ROOM_IDLE;
static uint32_t led_on_time = 0;
static uint32_t dutycycle = PWM_INITIAL_DUTY;
void room_control_app_init(void)
{
    // Inicializar PWM al duty cycle inicial (estado IDLE -> LED apagado)
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);
    uart_send_string("Controlador de Sala v2.0 Estado inicial: - Lampara: 20% - Puerta: Cerrada\r\n"); //punto 1
}

void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE) {
        current_state = ROOM_OCCUPIED;
        tim3_ch1_pwm_set_duty_cycle(100);  // PWM al 100%
        led_on_time = systick_get_ms();
        uart_send_string("Sala ocupada\r\n");
    } else {
        current_state = ROOM_IDLE;
        tim3_ch1_pwm_set_duty_cycle(0);  // PWM al 0%
        uart_send_string("Sala vacía\r\n");
    }
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("PWM: 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("PWM: 0%\r\n");
            break;
        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(20);
            uart_send_string("Estado actual: - Lámpara: 20% - Puerta: Cerrada\r\n");
            break;
        case 'S':
        case 's': //punto 2
            tim3_ch1_pwm_set_duty_cycle(dutycycle);
            if (current_state == ROOM_OCCUPIED) {
                uart_send_string("Estado actual: - Lámpara: %d%% - Puerta: Abierta\r\n", dutycycle);
            } else {
                uart_send_string("Estado actual: - Lámpara: %d%% - Puerta: Cerrada\r\n", dutycycle);
            }
        case '?': //punto 3
            tim3_ch1_pwm_set_duty_cycle(10);
            uart_send_string("Comandos disponibles:\r\n");          
            uart_send_string(" 1-5: Ajustar brillo lampara (10%, 20%, 30%, 40%, 50%)\r\n");
            uart_send_string(" 0: Apagar lampara\r\n");
            uart_send_string(" o: Abrir puerta (ocupar sala)\r\n");
            uart_send_string(" c: Cerrar puerta (vaciar sala)\r\n");
            uart_send_string(" s: Estado del sistema\r\n");
            uart_send_string(" ?: Ayuda\r\n");
            break;
        
        case 'g':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("PWM: 0%\r\n");
            break;
        case '1':
            tim3_ch1_pwm_set_duty_cycle(10);
            uart_send_string("PWM: 10%\r\n");
            break;
        case '2':
            tim3_ch1_pwm_set_duty_cycle(20);
            uart_send_string("PWM: 20%\r\n");
            break;
        case '3':
            tim3_ch1_pwm_set_duty_cycle(30);
            uart_send_string("PWM: 30%\r\n");
            break;
        case '4':
            tim3_ch1_pwm_set_duty_cycle(40);
            uart_send_string("PWM: 40%\r\n");
            break;
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            uart_send_string("PWM: 50%\r\n");
            break;
        default:
            uart_send_string("Comando desconocido: ");
            uart_send(received_char);
            uart_send_string("\r\n");
            break;
    }
}

void room_control_update(void) //punto 4
{
    tim3_ch1_pwm_set_duty_cycle(100);
    if (current_state == ROOM_OCCUPIED) {
        if (systick_get_ms() - led_on_time >= LED_TIMEOUT_MS) {
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(dutycycle);  // Volver al duty cycle definido
            uart_send_string("Timeout: Sala vacía\r\n");
        }
    }
}