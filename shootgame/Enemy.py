# -*- coding: utf-8 -*-
import pygame
from pygame.locals import *
from Screen import *
from Player import *

# 定义敌机对象使用的surface相关参数
enemy_rect = pygame.Rect(534, 612, 57, 43)
enemy_img = plane_img.subsurface(enemy_rect)
enemy_down_imgs_rect = [] # 存储敌机爆炸图片区域
enemy_down_imgs_rect.append(pygame.Rect(267, 347, 57, 43))
enemy_down_imgs_rect.append(pygame.Rect(873, 697, 57, 43))
enemy_down_imgs_rect.append(pygame.Rect(267, 296, 57, 43))
enemy_down_imgs_rect.append(pygame.Rect(930, 697, 57, 43))

#敌机击毁音
enemy_die_sound = pygame.mixer.Sound('resources/sound/enemy1_down.wav')

#敌人类
class Enemy(pygame.sprite.Sprite):
    def __init__(self,Screen,init_pos):
        pygame.sprite.Sprite.__init__(self) #创建父类
        self.screen = Screen
        self.image = enemy_img
        self.enemy_die_image = [] #存储敌机爆炸图片
        for i in range(len(enemy_down_imgs_rect)):
            self.enemy_die_image.append(plane_img.subsurface(enemy_down_imgs_rect[i]).convert_alpha())
        self.rect = self.image.get_rect()
        self.rect.topleft = init_pos
        self.speed = 2
        self.die_index = 0

    def Move(self):
        self.rect.top += self.speed

    def show_die_image(self,index=0):
        self.screen.screen.blit(self.enemy_die_image[index], self.rect)




