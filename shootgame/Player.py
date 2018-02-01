# -*- coding: utf-8 -*-
import pygame
from pygame.locals import *
from Screen import *

plane_img = pygame.image.load('resources/image/shoot.png')
player_die_img_rect = []
player_die_img_rect.append(pygame.Rect(165, 360, 102, 126)) # 玩家爆炸精灵图片区域
player_die_img_rect.append(pygame.Rect(165, 234, 102, 126))
player_die_img_rect.append(pygame.Rect(330, 624, 102, 126))
player_die_img_rect.append(pygame.Rect(330, 498, 102, 126))
player_die_img_rect.append(pygame.Rect(432, 624, 102, 126))

# 定义子弹对象使用的surface相关参数
bullet_img = plane_img.subsurface(pygame.Rect(1004, 987, 9, 21))
# 子弹音
bullet_sound = pygame.mixer.Sound('resources/sound/bullet.wav')



class Bullet(pygame.sprite.Sprite):
    def __init__(self,init_pos):
        pygame.sprite.Sprite.__init__(self)  # 创建父类
        self.image = bullet_img
        self.rect = self.image.get_rect()
        self.rect.midbottom = init_pos
        self.speed = 10

    def Move(self):
        self.rect.top -= self.speed



#玩家类 继承pygame自带的精灵类
class Player(pygame.sprite.Sprite):
    def __init__(self,Screen):
        pygame.sprite.Sprite.__init__(self) #创建父类
        self.screen = Screen
        self.rect = pygame.Rect(0, 99, 102, 126)  #玩家精灵图片区域大小
        self.image = plane_img.subsurface(self.rect).convert_alpha()       # 截取玩家精灵图片区域
        self.die_image = [] #存储玩家爆炸图片
        for i in range(len(player_die_img_rect)):
           self.die_image.append(plane_img.subsurface(player_die_img_rect[i]).convert_alpha())

        #玩家底部居中显示
        self.rect.top = self.screen.heigh - self.rect.height
        self.rect.left = (self.screen.width-self.rect.width)/2

        self.speed = 6  #玩家移动速度

        self.bullets = pygame.sprite.Group()  # 玩家飞机所发射的子弹的集合
        self.is_hit = False

    def shoot(self):
        bullet = Bullet(self.rect.midtop)
        self.bullets.add(bullet)
        bullet_sound.set_volume(0.2)
        bullet_sound.play()

    def MoveUp(self):
        if self.rect.top <= 0:
            self.rect.top = 0
            return
        self.rect.top -= self.speed
    def MoveDown(self):
        if self.rect.top >= (self.screen.heigh-self.rect.height):
            self.rect.top = self.screen.heigh - self.rect.height
            return
        self.rect.top += self.speed
    def MoveLeft(self):
        if self.rect.left <= 0:
            self.rect.left = 0
            return
        self.rect.left -= self.speed
    def MoveRight(self):
        if self.rect.left >= (self.screen.width-self.rect.width):
            self.rect.left = self.screen.width-self.rect.width
            return
        self.rect.left += self.speed

    def show_die_image(self,index=0):
        self.screen.screen.blit(self.die_image[index], self.rect)





