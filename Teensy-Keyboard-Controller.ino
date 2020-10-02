/*
 * 
 * TEENSY KEYBOARD CONTROLLER by LAK132
 *
 * This software defines two types of modifiers:
 *    * Modifier Keys eg. MODIFIERKEY_SHIFT
 *    * Modifier Scan Codes eg. 96
 * These different types of modifiers serve different purposes and great care should be taken not to get them mixed up
 *
 * This software was specifically designed for the Focus FK-2001 XT Mechanical Keyboard
 *
 */

#define defKeyboardCodes    {0, KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE, KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_ENTER   , MODIFIERKEY_CTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE, KEY_TILDE, MODIFIERKEY_SHIFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH   , MODIFIERKEY_SHIFT, KEY_PRINTSCREEN, MODIFIERKEY_ALT, KEY_SPACE, KEY_CAPS_LOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_NUM_LOCK, KEY_SCROLL_LOCK, KEYPAD_7, KEYPAD_8, KEYPAD_9   , KEYPAD_MINUS, KEYPAD_4, KEYPAD_5, KEYPAD_6 , KEYPAD_PLUS, KEYPAD_1, KEYPAD_2, KEYPAD_3     , KEYPAD_0  , KEYPAD_PERIOD, 0, 0, 0, KEY_F11, KEY_F12}
#define defAltKeyboardCodes {0, KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE, KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEYPAD_ENTER, MODIFIERKEY_CTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE, KEY_TILDE, MODIFIERKEY_SHIFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEYPAD_SLASH, MODIFIERKEY_SHIFT, KEYPAD_ASTERIX , MODIFIERKEY_ALT, KEY_SPACE, KEY_CAPS_LOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_NUM_LOCK, KEY_PAUSE      , KEY_HOME, KEY_UP  , KEY_PAGE_UP, KEYPAD_MINUS, KEY_LEFT, KEYPAD_5, KEY_RIGHT, KEYPAD_PLUS, KEY_END , KEY_DOWN, KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE   , 0, 0, 0, KEY_F11, KEY_F12}
                      
#define systemRequest 84
#define modCode 96
#define altModCode 97

#define clock digitalRead(clockPin)
#define data digitalRead(dataPin)

int clockPin = 10;
int dataPin = 23;
int switchPin = 22;
int keyTemp = 0;
int keyState[104];
int outKeyState[6] = {0, 0, 0, 0, 0, 0};
int outModState[4] = {0, 0, 0, 0};
int keyboardCode[] = defKeyboardCodes;
int altKeyboardCode[] = defAltKeyboardCodes;

boolean modCodeAct = false;
boolean altModCodeAct = false;

void setup()
{
  Serial.begin(9600);
  Serial.println("Connected");
  pinMode(clockPin, INPUT);
  pinMode(dataPin, INPUT);
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, HIGH);
  delay(1000);
  digitalWrite(switchPin, LOW); // Needed to start the Focus FK-2001 in AT mode before switching to XT mode
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.set_key2(0);
  Keyboard.set_key3(0);
  Keyboard.set_key4(0);
  Keyboard.set_key5(0);
  Keyboard.set_key6(0);
  Keyboard.send_now();
}

void loop()
{
  int x = 0;
  if (clock == LOW)
  {
    while (clock == LOW){}
    while (x < 8)
    {
      if (clock == HIGH)
      {
        if (x != 7)
        {
          keyTemp += (data << x);
        }
        else
        {
          keyState[keyTemp] = data;
        }
        x++;
        while (clock == HIGH){}
      }
      while (clock == LOW){}
    }
    //Code that requires the current key code goes here
    if (keyTemp == modCode || keyTemp == altModCode) // Allows the mod codes to register correctly
    {
      delayMicroseconds(100);
    }
    keyRegister (keyTemp, !keyState[keyTemp]); //Sends out the key presses every time it is run
    keyTemp = 0;
  }
}

// Registers all key presses

void keyRegister (int key, boolean pressed)
{
  if (modCodeAct)
  {
    if (altKeyboardCode[key] == MODIFIERKEY_CTRL || altKeyboardCode[key] == MODIFIERKEY_SHIFT || altKeyboardCode[key] == MODIFIERKEY_ALT || altKeyboardCode[key] == MODIFIERKEY_GUI)
    {
      setModKey (key, pressed, true);
    }  
    else
    {
      setNormKey (key, pressed, true);
    }
    modCodeAct = false;
  }
  else if (altModCodeAct)
  {
    if (altKeyboardCode[key] == MODIFIERKEY_CTRL || altKeyboardCode[key] == MODIFIERKEY_SHIFT || altKeyboardCode[key] == MODIFIERKEY_ALT || altKeyboardCode[key] == MODIFIERKEY_GUI)
    {
      setModKey (key, pressed, true);
    }
    else
    {
      setNormKey (key, pressed, true);
    }
    modCodeAct = true;
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
  else if (keyboardCode[key] == MODIFIERKEY_CTRL || keyboardCode[key] == MODIFIERKEY_SHIFT || keyboardCode[key] == MODIFIERKEY_ALT || keyboardCode[key] == MODIFIERKEY_GUI)
  {
    setModKey (key, pressed, false);
  }
  else
  {
    setNormKey (key, pressed, false);
  }
}

// Sets all normal key presses

void setNormKey (int key, boolean pressed, boolean mod)
{
  boolean set = false;
  int x = 0;
  int i = 0;
  while (i < 6 && !set)
  {
    if (outKeyState[i] == keyboardCode[key])
    {
      x = i; //Skips x to the point where the key is already pressed to prevent key from being registered twice (ghosting bug)
      set = true;
    }
    i++;
  }
  set = false;
  while (x < 6 && !set)
  {
    if ((outKeyState[x] == keyboardCode[key] && pressed && !mod) || (outKeyState[x] == altKeyboardCode[key] && pressed && mod))
    {
      set = true;
    }
    else if (((((outKeyState[x] == 0) && pressed) || ((outKeyState[x] == keyboardCode[key]) && !pressed)) && !mod) || ((((outKeyState[x] == 0) && pressed) || ((outKeyState[x] == altKeyboardCode[key]) && !pressed)) && mod))
    {
      if (pressed && !mod)
      {
        outKeyState[x] = keyboardCode[key];
      }
      else if (pressed && mod)
      {
        outKeyState[x] = altKeyboardCode[key];
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

void setModKey (int key, boolean pressed, int mod)
{
  boolean set = false;
  int x = 0;
  int i = 0;
  while (i < 4 && !set)
  {
    if (outKeyState[i] == keyboardCode[key])
    {
      x = i; //Skips x to the point where the key is already pressed to prevent key from being registered twice (ghosting bug)
      set = true;
    }
    i++;
  }
  set = false;
  while (x < 4 && !set)
  {
    if ((outModState[x] == keyboardCode[key] && pressed && !mod) || (outModState[x] == altKeyboardCode[key] && pressed && mod))
    {
      set = true;
    }
    else if (((((outModState[x] == 0) && pressed) || ((outModState[x] == keyboardCode[key]) && !pressed)) && !mod) || ((((outModState[x] == 0) && pressed) || ((outModState[x] == altKeyboardCode[key]) && !pressed)) && mod))
    {
      if (pressed && !mod)
      {
        outModState[x] = keyboardCode[key];
      }
      else if (pressed && mod)
      {
        outModState[x] = altKeyboardCode[key];
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
  Keyboard.set_modifier(outModState[0] | outModState[1] | outModState[2] | outModState[3]);
  Keyboard.send_now();
}
