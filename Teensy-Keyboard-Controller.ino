/*
 *
 * TEENSY KEYBOARD CONTROLLER by LAK132
 *
 * This software defines two types of modifiers:
 *        * Modifier Keys eg. MODIFIERKEY_SHIFT
 *        * Modifier Scan Codes eg. 96
 * These different types of modifiers serve different purposes and great care
 * should be taken not to get them mixed up
 *
 * This software was specifically designed for the Focus FK-2001 XT Mechanical
 * Keyboard
 *
 */

#include <Arduino.h>
#include <HardwareSerial.h>

#include "keyboard-map.hpp"

#define systemRequest 84
#define modCode       96
#define altModCode    97

#define CLOCK_PIN  10
#define DATA_PIN   23
#define SWITCH_PIN 22

#define CLOCK digitalRead(CLOCK_PIN)
#define DATA  digitalRead(DATA_PIN)

bool keyState[104];
int outKeyState[6] = {0, 0, 0, 0, 0, 0};
int outModState[4] = {0, 0, 0, 0};

bool modCodeAct    = false;
bool altModCodeAct = false;

void setup()
{
  Serial.begin(19200);
  Serial.println("Connected");
  pinMode(CLOCK_PIN, INPUT);
  pinMode(DATA_PIN, INPUT);
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, LOW);
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.set_key2(0);
  Keyboard.set_key3(0);
  Keyboard.set_key4(0);
  Keyboard.set_key5(0);
  Keyboard.set_key6(0);
  Keyboard.send_now();
  while (CLOCK == LOW)
    ;
}

void loop()
{
  unsigned char data = 0;

  unsigned long t = micros();
  while (CLOCK == HIGH && (micros() - t) < 200)
    ;

  if (CLOCK != HIGH) return; // out of sync

  while (CLOCK == HIGH)
    ;

  for (;;)
  {
    while (CLOCK == LOW)
      ;

    int v = DATA;

    t = micros();
    while (CLOCK == HIGH && (micros() - t) < 50)
      ;

    if (CLOCK == HIGH) break;
    data >>= 1;
    data |= v ? 0b10000000U : 0b00000000U;
  }

  int scancode = data & 0b01111111U;

  keyState[scancode] = (data & 0b10000000) != 0;

  keyRegister(scancode, !keyState[scancode]);
}

// Registers all key presses

void keyRegister(int key, boolean pressed)
{
  if (modCodeAct)
  {
    if (numlock_keyboard_code[key] == MODIFIERKEY_CTRL ||
        numlock_keyboard_code[key] == MODIFIERKEY_SHIFT ||
        numlock_keyboard_code[key] == MODIFIERKEY_ALT ||
        numlock_keyboard_code[key] == MODIFIERKEY_GUI)
    {
      setModKey(key, pressed, true);
    }
    else
    {
      setNormKey(key, pressed, true);
    }
    modCodeAct = false;
  }
  else if (altModCodeAct)
  {
    if (numlock_keyboard_code[key] == MODIFIERKEY_CTRL ||
        numlock_keyboard_code[key] == MODIFIERKEY_SHIFT ||
        numlock_keyboard_code[key] == MODIFIERKEY_ALT ||
        numlock_keyboard_code[key] == MODIFIERKEY_GUI)
    {
      setModKey(key, pressed, true);
    }
    else
    {
      setNormKey(key, pressed, true);
    }
    modCodeAct    = true;
    altModCodeAct = false;
  }
  else if (key == modCode)
  {
    modCodeAct = true;
  }
  else if (key == altModCode)
  {
    altModCodeAct = true;
  }
  else if (keyboard_code[key] == MODIFIERKEY_CTRL ||
           keyboard_code[key] == MODIFIERKEY_SHIFT ||
           keyboard_code[key] == MODIFIERKEY_ALT ||
           keyboard_code[key] == MODIFIERKEY_GUI)
  {
    setModKey(key, pressed, false);
  }
  else
  {
    setNormKey(key, pressed, false);
  }
}

// Sets all normal key presses

void setNormKey(int key, boolean pressed, boolean mod)
{
  boolean set = false;
  int x       = 0;
  int i       = 0;
  while (i < 6 && !set)
  {
    if (outKeyState[i] == keyboard_code[key])
    {
      x = i; // Skips x to the point where the key is already pressed to
             // prevent key from being registered twice (ghosting bug)
      set = true;
    }
    i++;
  }
  set = false;
  while (x < 6 && !set)
  {
    if ((outKeyState[x] == keyboard_code[key] && pressed && !mod) ||
        (outKeyState[x] == numlock_keyboard_code[key] && pressed && mod))
    {
      set = true;
    }
    else if (((((outKeyState[x] == 0) && pressed) ||
               ((outKeyState[x] == keyboard_code[key]) && !pressed)) &&
              !mod) ||
             ((((outKeyState[x] == 0) && pressed) ||
               ((outKeyState[x] == numlock_keyboard_code[key]) && !pressed)) &&
              mod))
    {
      if (pressed && !mod)
      {
        outKeyState[x] = keyboard_code[key];
      }
      else if (pressed && mod)
      {
        outKeyState[x] = numlock_keyboard_code[key];
      }
      else
      {
        outKeyState[x] = 0;
      }
      Serial.println(outKeyState[x]);
      set = true;
    }
    x++;
  }
  Keyboard.set_key1(outKeyState[0]);
  Keyboard.set_key2(outKeyState[1]);
  Keyboard.set_key3(outKeyState[2]);
  Keyboard.set_key4(outKeyState[3]);
  Keyboard.set_key5(outKeyState[4]);
  Keyboard.set_key6(outKeyState[5]);
  Keyboard.send_now();
}

// Sets all modifier key presses

void setModKey(int key, boolean pressed, int mod)
{
  boolean set = false;
  int x       = 0;
  int i       = 0;
  while (i < 4 && !set)
  {
    if (outKeyState[i] == keyboard_code[key])
    {
      x = i; // Skips x to the point where the key is already pressed to
             // prevent key from being registered twice (ghosting bug)
      set = true;
    }
    i++;
  }
  set = false;
  while (x < 4 && !set)
  {
    if ((outModState[x] == keyboard_code[key] && pressed && !mod) ||
        (outModState[x] == numlock_keyboard_code[key] && pressed && mod))
    {
      set = true;
    }
    else if (((((outModState[x] == 0) && pressed) ||
               ((outModState[x] == keyboard_code[key]) && !pressed)) &&
              !mod) ||
             ((((outModState[x] == 0) && pressed) ||
               ((outModState[x] == numlock_keyboard_code[key]) && !pressed)) &&
              mod))
    {
      if (pressed && !mod)
      {
        outModState[x] = keyboard_code[key];
      }
      else if (pressed && mod)
      {
        outModState[x] = numlock_keyboard_code[key];
      }
      else
      {
        outModState[x] = 0;
      }
      set = true;
    }
    x++;
  }
  Serial.println(outModState[0]);
  Keyboard.set_modifier(outModState[0] | outModState[1] | outModState[2] |
                        outModState[3]);
  Keyboard.send_now();
}
