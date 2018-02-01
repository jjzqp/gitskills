# -*- coding: utf-8 -*-
import pygame
import time
import random
from Screen import *
Scr = Screen()
from Player import *
from Enemy import *
player = Player(Scr)
#刷出玩家初始位置
Scr.screen.blit(player.image, player.rect)
pygame.display.update()
die_index = 0
shoot_frequency = 0
enemies_frequency = 0

score = 0
#产生的敌机集合
enemies = pygame.sprite.Group()
enemies_die = pygame.sprite.Group()

while True:
    # 控制游戏最大帧率为60
    pygame.time.Clock().tick(60)



    # 更新屏幕
    Scr.screen.fill(0)
    Scr.screen.blit(Scr.backgroud, (0, 0))
    Scr.screen.blit(player.image, player.rect)

    #生成子弹
    if shoot_frequency % 15 == 0:
        player.shoot()
    shoot_frequency += 1
    if shoot_frequency >= 15:
        shoot_frequency = 0

    # 移动子弹，若超出窗口范围则删除
    for bullet in player.bullets:
        bullet.Move()
        if bullet.rect.bottom <= 0:
            player.bullets.remove(bullet)

    player.bullets.draw(Scr.screen)

    #生成敌机
    if enemies_frequency % 50 == 0:
        enemy_pos = [random.randint(0, Scr.width - enemy_rect.width), 0]
        enemy = Enemy(Scr,enemy_pos)
        enemies.add(enemy)
    enemies_frequency += 1
    if enemies_frequency >= 100:
        enemies_frequency = 0

    # 移动敌机，若超出窗口范围则删除
    for e in enemies:
        e.Move()
        if e.rect.bottom >= Scr.heigh:
            enemies.remove(e)
        if pygame.sprite.collide_circle(e, player):
            #判断敌机是否击中玩家
            player.is_hit = True
            enemies.remove(e)
            enemies_die.add(e)
            Scr.game_over_play_sound()
            break
    enemies.draw(Scr.screen)

    # 将被击中的敌机对象添加到击毁敌机Group中，用来渲染击毁动画
    enemies1_die = pygame.sprite.groupcollide(enemies, player.bullets, 1, 1)
    for enemy_die in enemies1_die:
        enemies_die.add(enemy_die)

    for enemy_die in enemies_die:
        if enemy_die.die_index == 0:
            enemy_die_sound.play()
        enemy_die.show_die_image(enemy_die.die_index)
        enemy_die.die_index += 1
        if enemy_die.die_index == 4:
            enemies_die.remove(enemy_die)
            score += 100


    if player.is_hit:
        #玩家被击中,游戏结束
        player.show_die_image(die_index)
        die_index += 1
        if die_index == 5:
            break

    # 绘制得分
    score_font = pygame.font.Font(None, 36)
    score_text = score_font.render(str(score), True, (128, 128, 128))
    text_rect = score_text.get_rect()
    text_rect.topleft = [10, 10]
    Scr.screen.blit(score_text, text_rect)

    pygame.display.update() #刷新整个画面,非常重要,没有这个画面将无法更新渲染

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

    # 监听键盘事件
    key_pressed = pygame.key.get_pressed()
    # 若玩家被击中，则无效
    if key_pressed[K_w] or key_pressed[K_UP]:
        player.MoveUp()
    if key_pressed[K_s] or key_pressed[K_DOWN]:
        player.MoveDown()
    if key_pressed[K_a] or key_pressed[K_LEFT]:
        player.MoveLeft()
    if key_pressed[K_d] or key_pressed[K_RIGHT]:
        player.MoveRight()

# Game Over !!!
time.sleep(1)
font = pygame.font.Font(None, 48)
text = font.render('Score: '+ str(score), True, (255, 0, 0))
text_rect = text.get_rect()
text_rect.centerx = Scr.screen.get_rect().centerx
text_rect.centery = Scr.screen.get_rect().centery - 100
Scr.screen_game_over()
Scr.screen.blit(text, text_rect)
pygame.display.update()

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
