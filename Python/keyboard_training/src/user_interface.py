import user_iteraction
import tkinter
import time

class UserInterface:
    '''Class of user interface of training'''
    def __init__(self):
        self.is_start = False

        self.user_iter = user_iteraction.UserIteraction()

        '''Create window'''
        self.window = tkinter.Tk()
        self.window.title("Keyboard training")
        self.window.geometry("720x480")
        self.window.resizable(False, False)
        self.window["bg"] = "#91A0FF"

        '''Create button of start typing'''
        self.start_button = tkinter.Button(self.window,
                                           text="Start typing",
                                           font="Consolas 14",
                                           border=0,
                                           background="#F3C4A0",
                                           activebackground="#FAD4B0",
                                           command=self.start_game)
        self.start_button.configure(width=25, height=2)
        self.start_button.pack()
        self.start_button.place(x=215, y=300)

        '''Create exit button'''
        self.exit_button = tkinter.Button(self.window,
                                          text="Exit",
                                          font="Consolas 14",
                                          border=0,
                                          background="#F3C4A0",
                                          activebackground="#FAD4B0",
                                          command=self.window.destroy)
        self.exit_button.configure(width=25, height=1)
        self.exit_button.pack()
        self.exit_button.place(x=215, y=370)

        '''Create button of change current language to english'''
        self.english_button = tkinter.Button(self.window,
                                             text="Eng",
                                             font="Consolas 14",
                                             border=0,
                                             background="#F3C4A0",
                                             activebackground="#FAD4B0",
                                             command=self.english_but)
        self.english_button.configure(width=10, height=1)
        self.english_button.pack()
        self.english_button.place(x=215, y=413)

        '''Create button of change current language to russian'''
        self.russian_button = tkinter.Button(self.window,
                                             text="Рус",
                                             font="Consolas 14",
                                             border=0,
                                             background="#F3C4A0",
                                             activebackground="#FAD4B0",
                                             command=self.russian_but)
        self.russian_button.configure(width=10, height=1)
        self.russian_button.pack()
        self.russian_button.place(x=380, y=413)

    def start_game(self):
        '''Reset params'''
        self.user_iter.words_count = 0
        self.user_iter.mistake_count = 0
        self.user_iter.generate_text()

        if self.is_start:
            self.user_iter.text_label.destroy()
            self.user_iter.mistakes_label.destroy()
            self.user_iter.words_label.destroy()

        '''Draw text for typing'''
        self.user_iter.text_label = tkinter.Label(self.window,
                                                  text=self.user_iter.get_text(),
                                                  font="Consolas 14",
                                                  background="#8190EF")
        self.user_iter.text_label.pack()
        self.user_iter.text_label.place(x=10, y=100)

        '''Draw mistake counter'''
        self.user_iter.mistakes_label = tkinter.Label(self.window,
                                                      text=f"Mistakes count: {self.user_iter.mistake_count}",
                                                      font="Consolas 14",
                                                      background="#91A0FF")
        self.user_iter.mistakes_label.pack()
        self.user_iter.mistakes_label.place(x=10, y=30)

        '''Draw words counter'''
        self.user_iter.words_label = tkinter.Label(self.window,
                                                   text=f"Words count: {self.user_iter.words_count}",
                                                   font="Consolas 14",
                                                   background="#91A0FF")
        self.user_iter.words_label.pack()
        self.user_iter.words_label.place(x=10, y=60)

        '''Bind to press'''
        self.window.bind("<KeyPress>", self.user_iter.key)

        self.user_iter.current_time = time.perf_counter()

        '''Delete eng and rus buttons'''
        self.english_button.destroy()
        self.russian_button.destroy()

        self.start_button["text"] = "Reset"

        self.is_start = True

    '''Bindings for buttons'''
    def english_but(self):
        self.user_iter.current_language = "english"

    def russian_but(self):
        self.user_iter.current_language = "russian"

    def display(self):
        self.window.mainloop()