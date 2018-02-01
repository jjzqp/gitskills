# -*- coding: utf-8 -*-
import pygame


class Screen():
    def __init__(self):
        self.width = 480
        self.heigh = 640
        pygame.init()
        self.screen = pygame.display.set_mode((self.width, self.heigh))
        pygame.display.set_caption("Air Shoot")
        self.backgroud = pygame.image.load('resources/image/background.png').convert()
        self.screen.fill(0)
        self.screen.blit(self.backgroud, (0, 0))
        pygame.display.update()

        #backgroud music
        pygame.mixer.music.load('resources/sound/game_music.wav')
        pygame.mixer.music.play(-1, 0.0)
        pygame.mixer.music.set_volume(0.25)

    def screen_set_width_heigh(self, width, heigh):
        self.width = width
        self.heigh = heigh
        self.screen = pygame.display.set_mode((self.width, self.heigh))
        self.screen.fill(0)
        if isinstance(self.backgroud,tuple):
            self.screen.fill(self.backgroud)
        else:
            self.screen.blit(self.backgroud, (0, 0))
        pygame.display.update()

    def screen_set_backgroud(self,backgroud):
        self.backgroud = backgroud
        self.screen.fill(backgroud)
        pygame.display.update()

    def screen_set_title(self,title):
        pygame.display.set_caption(title)

    def screen_game_over(self):
        self.game_over_img = pygame.image.load('resources/image/gameover.png').convert()
        self.screen.blit(self.game_over_img, (0, 0))

    def game_over_play_sound(self):
        #game over music
        self.game_over_sound = pygame.mixer.Sound('resources/sound/game_over.wav')
        self.game_over_sound.set_volume(0.3)
        self.game_over_sound.play()


#test module
#Screen = Screen()
#while True:
#    a = raw_input("input:")
#    if a == 'a':
#        Screen.screen_set_width_heigh(200,200)
#    elif a == 'b':
#        Screen.screen_set_title("feijidazhan")
#    elif a == 'c':
#        Screen.screen_set_backgroud((24,7,78))
#    elif a == 'g':
#        Screen.screen_game_over()
#    pass
