#import serial
import time
from kivy.app import App
from kivy.uix.gridlayout import GridLayout
from kivy.uix.label import Label
from kivy.uix.image import Image
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput
from kivy.uix.checkbox import CheckBox

import serial

s = serial.Serial("COM6", 9600)


class LedMatrixControlApp(App):
    def build(self):
        self.window = GridLayout()
        self.window.cols = 8
        self.window.rows = 6

        self.window.size_hint = (.8, .9)
        self.window.pos_hint = {"center_x": .5, "center_y": .5}

        self.ch1 = CheckBox()
        self.ch1.bind(active=self.ch1_callback)
        self.window.add_widget(self.ch1)
        self.window.add_widget(Label(text='Matryca 1'))

        self.ch2 = CheckBox()
        self.ch2.bind(active=self.ch2_callback)
        self.window.add_widget(self.ch2)
        self.window.add_widget(Label(text='Matryca 2'))

        self.ch3 = CheckBox()
        self.ch3.bind(active=self.ch3_callback)
        self.window.add_widget(self.ch3)
        self.window.add_widget(Label(text='Matryca 3'))

        self.ch4 = CheckBox()
        self.ch4.bind(active=self.ch4_callback)
        self.window.add_widget(self.ch4)
        self.window.add_widget(Label(text='Matryca 4'))

        self.heart = Button(background_normal='graphics/heart.png', background_down='graphics/heart.png')
        self.heart.bind(on_press=self.heart_btn_callback)
        self.window.add_widget(self.heart)

        black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
        self.window.add_widget(black_btn)

        self.dices = Button(background_normal='graphics/dices.png', background_down='graphics/dices.png')
        self.dices.bind(on_press=self.dices_btn_callback)
        self.window.add_widget(self.dices)
        black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
        self.window.add_widget(black_btn)

        self.stairs = Button(background_normal='graphics/stairs.png', background_down='graphics/stairs.png')
        self.stairs.bind(on_press=self.stairs_btn_callback)
        self.window.add_widget(self.stairs)
        black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
        self.window.add_widget(black_btn)

        self.pacman = Button(background_normal='graphics/pacman.png', background_down='graphics/pacman.png')
        self.pacman.bind(on_press=self.pacman_btn_callback)
        self.window.add_widget(self.pacman)

        for i in range(9):
            black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
            self.window.add_widget(black_btn)

        self.draw = Button(background_normal='graphics/60.png', background_down='graphics/60.png')
        self.draw.bind(on_press=self.draw_btn_callback)
        self.window.add_widget(self.draw)
        black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
        self.window.add_widget(black_btn)

        self.rand = Button(background_normal='graphics/rand.png', background_down='graphics/rand.png')
        self.rand.bind(on_press=self.random_btn_callback)
        self.window.add_widget(self.rand)

        for i in range(13):
            black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
            self.window.add_widget(black_btn)

        self.text_mode = Button(size_hint=(1, 1), text='TEXT MODE')
        self.text_mode.bind(on_press=self.text_mode_btn_callback)
        self.window.add_widget(self.text_mode)

        self.input = TextInput(size_hint=(1, .5))
        self.window.add_widget(self.input)

        black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
        self.window.add_widget(black_btn)

        self.text = Button(size_hint=(1, 1), text='ZATWIERDŹ')
        self.text.bind(on_press=self.text_btn_callback)
        self.window.add_widget(self.text)

        for i in range(3):
            black_btn = Button(background_normal='graphics/black.png', background_down='graphics/black.png')
            self.window.add_widget(black_btn)

        self.clear = Button(size_hint=(1, .5), text='CLEAR')
        self.clear.bind(on_press=self.clear_btn_callback)
        self.window.add_widget(self.clear)

        return self.window

    def ch1_callback(self, checkbox, v):
        s.write(b'ch4')

    def ch2_callback(self, checkbox, v):
        s.write(b'ch3')

    def ch3_callback(self, checkbox, v):
        s.write(b'ch2')

    def ch4_callback(self, checkbox, v):
        s.write(b'ch1')

    def heart_btn_callback(self, instance):
        s.write(b'heart')

    def dices_btn_callback(self, instance):
        s.write(b'dice')

    def stairs_btn_callback(self, instance):
        s.write(b'stairs')

    def pacman_btn_callback(self, instance):
        s.write(b'pacman')

    def draw_btn_callback(self, instance):
        s.write(b'draw')

    def random_btn_callback(self, instance):
        s.write(b'random')

    def text_mode_btn_callback(self, instance):
        s.write(b'text')

    def text_btn_callback(self, instance):
        s.write(b'*' + self.input.text.encode())

    def clear_btn_callback(self, instance):
        self.ch1.active = False
        self.ch2.active = False
        self.ch3.active = False
        self.ch4.active = False
        s.write(b'clear')



if __name__ == '__main__':
    LedMatrixControlApp().run()
    s.close()