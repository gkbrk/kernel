#pragma once

static void pcspeaker_play_sound(uint32_t nFrequence) {
  uint32_t Div;
  uint8_t tmp;

  // Set the PIT to the desired frequency
  Div = 1193180 / nFrequence;
  outb(0x43, 0xb6);
  outb(0x42, (uint8_t)(Div));
  outb(0x42, (uint8_t)(Div >> 8));

  // And play the sound using the PC speaker
  tmp = inb(0x61);
  if (tmp != (tmp | 3)) {
    outb(0x61, tmp | 3);
  }
}

static void pcspeaker_nosound() {
  uint8_t tmp = inb(0x61) & 0xFC;

  outb(0x61, tmp);
}

void speaker_task() {
  while (1) {
    Message *m = message_get(&runningTask->port);
    klog(m->message);

    char *cmd = strsep(&m->message, " ");

    if (streq(cmd, "freq")) {
      int freq = atoi(strsep(&m->message, " "));
      pcspeaker_play_sound(freq);
    } else if (streq(cmd, "stop")) {
      pcspeaker_nosound();
    }

    m->response = "";
    yield();
  }
}

bool pcspeaker_init() {
  spawnTask(speaker_task, "speaker-driver");

  pcspeaker_play_sound(1000);
  for (int i = 0; i < 9999; i++)
    yield(); // Dumb delay
  pcspeaker_nosound();

  return true;
}

driverDefinition PCSPEAKER_DRIVER = {.name = "PC Speaker",
                                     .isAvailable = driver_true,
                                     .initialize = pcspeaker_init};
