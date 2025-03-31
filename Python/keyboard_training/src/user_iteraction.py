import global_var
import tkinter
import time
import random

class UserIteraction:
    '''Class for iteraction between user and training'''
    def __init__(self):
        '''Default values'''
        self.current_text = list()
        self.current_language = "english"
        self.words_count = 0
        self.mistake_count = 0
        self.current_time = 0
        self.text_label = {}
        self.mistakes_label = {}
        self.words_label = {}

    '''Generate new text for typing'''
    def generate_text(self):
        self.current_text.clear()

        if self.current_language == "english":
            for _ in range(30):
                self.current_text.append(
                    global_var.global_english_words[random.randint(0, len(global_var.global_english_words) - 1)])
        else:
            for _ in range(30):
                self.current_text.append(
                    global_var.global_russian_words[random.randint(0, len(global_var.global_russian_words) - 1)])

    '''Get time from start'''
    def get_current_time(self, timer):
        return timer - self.current_time

    '''Text for typing'''
    def get_text(self):
       text = ""
       str_len = 0
       for word in self.current_text:
           if str_len > 45:
               text += '\n'
               str_len = 0
           text += word + " "
           str_len += len(word)
       return text

    '''Bind function for user's press key'''
    def key(self, event):
        is_time_ok = True
        if self.get_current_time(time.perf_counter()) > 60:
            self.text_label["text"] = "Your time is finished"
            is_time_ok = False

        if is_time_ok and len(self.current_text) > 0:
            if is_time_ok and event.char == self.current_text[0][0]:
                if len(self.current_text[0]) == 1:
                    self.words_count += 1
                    self.words_label["text"] = f"Words count: {self.words_count}"
                    self.current_text.pop(0)
                else:
                    self.current_text[0] = self.current_text[0][1::]

                self.text_label["text"] = self.get_text()
            else:
                self.mistake_count += 1
                self.mistakes_label["text"] = f"Mistakes count: {self.mistake_count}"
        else:
            self.text_label["text"] = "You are greate!"