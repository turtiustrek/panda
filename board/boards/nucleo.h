// /////////// //
// nucleo Panda //
// /////////// //

void nucleo_enable_can_transceiver(uint8_t transceiver, bool enabled) {
  switch (transceiver){
    case 1U:
      set_gpio_output(GPIOC, 1, !enabled);
      break;
    case 2U:
      set_gpio_output(GPIOC, 13, !enabled);
      break;
    case 3U:
      set_gpio_output(GPIOA, 0, !enabled);
      break;
    default:
      puts("Invalid CAN transceiver ("); puth(transceiver); puts("): enabling failed\n");
      break;
  }
}

void nucleo_enable_can_transceivers(bool enabled) {
  uint8_t t1 = enabled ? 1U : 2U;  // leave transceiver 1 enabled to detect CAN ignition
  for(uint8_t i=t1; i<=3U; i++) {
    nucleo_enable_can_transceiver(i, enabled);
  }
}

void nucleo_set_led(uint8_t color, bool enabled) {
  switch (color){
    case LED_RED:
      set_gpio_output(GPIOB,14, enabled);
      break;
     case LED_GREEN:
      set_gpio_output(GPIOB,0, enabled);
      break;
    case LED_BLUE:
      set_gpio_output(GPIOB,7, enabled);
      break;
    default:
      break;
  }
}

void nucleo_set_usb_power_mode(uint8_t mode){
  bool valid_mode = true;
  switch (mode) {
    case USB_POWER_CLIENT:
      // B2,A13: set client mode
      set_gpio_output(GPIOB, 2, 0);
      set_gpio_output(GPIOA, 13, 1);
      break;
    case USB_POWER_CDP:
      // B2,A13: set CDP mode
      set_gpio_output(GPIOB, 2, 1);
      set_gpio_output(GPIOA, 13, 1);
      break;
    case USB_POWER_DCP:
      // B2,A13: set DCP mode on the charger (breaks USB!)
      set_gpio_output(GPIOB, 2, 0);
      set_gpio_output(GPIOA, 13, 0);
      break;
    default:
      valid_mode = false;
      puts("Invalid usb power mode\n");
      break;
  }

  if (valid_mode) {
    usb_power_mode = mode;
  }
}

void nucleo_set_gps_mode(uint8_t mode) {
  switch (mode) {
    case GPS_DISABLED:
      // ESP OFF
      set_gpio_output(GPIOC, 14, 0);
      set_gpio_output(GPIOC, 5, 0);
      break;
    case GPS_BOOTMODE:
      set_gpio_output(GPIOC, 14, 1);
      set_gpio_output(GPIOC, 5, 0);
      break;
    default:
      puts("Invalid ESP/GPS mode\n");
      break;
  }
}

void nucleo_set_can_mode(uint8_t mode){
  switch (mode) {
    case CAN_MODE_NORMAL:
      // B12,B13: disable GMLAN mode
      set_gpio_mode(GPIOB, 12, MODE_INPUT);
      set_gpio_mode(GPIOB, 13, MODE_INPUT);

      // B3,B4: disable GMLAN mode
      set_gpio_mode(GPIOB, 3, MODE_INPUT);
      set_gpio_mode(GPIOB, 4, MODE_INPUT);

      // B5,B6: normal CAN2 mode
      set_gpio_alternate(GPIOB, 5, GPIO_AF9_CAN2);
      set_gpio_alternate(GPIOB, 6, GPIO_AF9_CAN2);

      // A8,A15: normal CAN3 mode
      set_gpio_alternate(GPIOA, 8, GPIO_AF11_CAN3);
      set_gpio_alternate(GPIOA, 15, GPIO_AF11_CAN3);
      break;
    case CAN_MODE_GMLAN_CAN2:
      // B5,B6: disable CAN2 mode
      set_gpio_mode(GPIOB, 5, MODE_INPUT);
      set_gpio_mode(GPIOB, 6, MODE_INPUT);

      // B3,B4: disable GMLAN mode
      set_gpio_mode(GPIOB, 3, MODE_INPUT);
      set_gpio_mode(GPIOB, 4, MODE_INPUT);

      // B12,B13: GMLAN mode
      set_gpio_alternate(GPIOB, 12, GPIO_AF9_CAN2);
      set_gpio_alternate(GPIOB, 13, GPIO_AF9_CAN2);

      // A8,A15: normal CAN3 mode
      set_gpio_alternate(GPIOA, 8, GPIO_AF11_CAN3);
      set_gpio_alternate(GPIOA, 15, GPIO_AF11_CAN3);
      break;
    case CAN_MODE_GMLAN_CAN3:
      // A8,A15: disable CAN3 mode
      set_gpio_mode(GPIOA, 8, MODE_INPUT);
      set_gpio_mode(GPIOA, 15, MODE_INPUT);

      // B12,B13: disable GMLAN mode
      set_gpio_mode(GPIOB, 12, MODE_INPUT);
      set_gpio_mode(GPIOB, 13, MODE_INPUT);

      // B3,B4: GMLAN mode
      set_gpio_alternate(GPIOB, 3, GPIO_AF11_CAN3);
      set_gpio_alternate(GPIOB, 4, GPIO_AF11_CAN3);

      // B5,B6: normal CAN2 mode
      set_gpio_alternate(GPIOB, 5, GPIO_AF9_CAN2);
      set_gpio_alternate(GPIOB, 6, GPIO_AF9_CAN2);
      break;
    default:
      puts("Tried to set unsupported CAN mode: "); puth(mode); puts("\n");
      break;
  }
}

uint32_t nucleo_read_current(void){
  return adc_get(ADCCHAN_CURRENT);
}

bool nucleo_check_ignition(void){
  // ignition is on PA1
  return !get_gpio_input(GPIOA, 1);
}

void nucleo_grey_common_init(void) {
  common_init_gpio();

  // C3: current sense
  set_gpio_mode(GPIOC, 3, MODE_ANALOG);

  // A1: started_alt
  set_gpio_pullup(GPIOA, 1, PULL_UP);

  // A2, A3: USART 2 for debugging
  set_gpio_alternate(GPIOD, 8, GPIO_AF7_USART3);
  set_gpio_alternate(GPIOD, 9, GPIO_AF7_USART3);

  // A4, A5, A6, A7: SPI
  set_gpio_alternate(GPIOA, 4, GPIO_AF5_SPI1);
  set_gpio_alternate(GPIOA, 5, GPIO_AF5_SPI1);
  set_gpio_alternate(GPIOA, 6, GPIO_AF5_SPI1);
  set_gpio_alternate(GPIOA, 7, GPIO_AF5_SPI1);

  // B12: GMLAN, ignition sense, pull up
  set_gpio_pullup(GPIOB, 12, PULL_UP);

  /* GMLAN mode pins:
      M0(B15)  M1(B14)  mode
      =======================
      0        0        sleep
      1        0        100kbit
      0        1        high voltage wakeup
      1        1        33kbit (normal)
  */
  set_gpio_output(GPIOB, 14, 1);
  set_gpio_output(GPIOB, 15, 1);

  // B7: K-line enable
  set_gpio_output(GPIOB, 7, 1);

  // C12, D2: Setup K-line (UART5)
  set_gpio_alternate(GPIOC, 12, GPIO_AF8_UART5);
  set_gpio_alternate(GPIOD, 2, GPIO_AF8_UART5);
  set_gpio_pullup(GPIOD, 2, PULL_UP);

  // L-line enable
  set_gpio_output(GPIOA, 14, 1);

  // C10, C11: L-Line setup (USART3)
  set_gpio_alternate(GPIOA, 2, GPIO_AF7_USART2);
  set_gpio_alternate(GPIOA, 3, GPIO_AF7_USART2);
  set_gpio_pullup(GPIOC, 11, PULL_UP);

  // Enable CAN transceivers
  nucleo_enable_can_transceivers(true);

  // Disable LEDs
  nucleo_set_led(LED_RED, false);
  nucleo_set_led(LED_GREEN, false);
  nucleo_set_led(LED_BLUE, false);

  // Set normal CAN mode
  nucleo_set_can_mode(CAN_MODE_NORMAL);

  // Init usb power mode
  uint32_t voltage = adc_get_voltage();
  // Init in CDP mode only if panda is powered by 12V.
  // Otherwise a PC would not be able to flash a standalone panda
  if (voltage > 8000U) {  // 8V threshold
    nucleo_set_usb_power_mode(USB_POWER_CDP);
  } else {
    nucleo_set_usb_power_mode(USB_POWER_CLIENT);
  }
}

void nucleo_init(void) {
  nucleo_grey_common_init();

  // Set ESP off by default
  current_board->set_gps_mode(GPS_DISABLED);
}

const harness_configuration nucleo_harness_config = {
  .has_harness = false
};

const board board_nucleo = {
  .board_type = "nucleo",
  .harness_config = &nucleo_harness_config,
  .has_gps = false,
  .has_hw_gmlan = false,
  .has_obd = false,
  .has_lin = false,
  .has_rtc = false,
  .init = nucleo_init,
  .enable_can_transceiver = nucleo_enable_can_transceiver,
  .enable_can_transceivers = nucleo_enable_can_transceivers,
  .set_led = nucleo_set_led,
  .set_usb_power_mode = nucleo_set_usb_power_mode,
  .set_gps_mode = nucleo_set_gps_mode,
  .set_can_mode = nucleo_set_can_mode,
  .usb_power_mode_tick = unused_usb_power_mode_tick,
  .check_ignition = nucleo_check_ignition,
  .read_current = nucleo_read_current,
  .set_fan_power = unused_set_fan_power,
  .set_ir_power = unused_set_ir_power,
  .set_phone_power = unused_set_phone_power,
  .set_clock_source_mode = unused_set_clock_source_mode,
  .set_siren = unused_set_siren
};
