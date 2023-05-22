from django.db import models
from django.contrib.auth import get_user_model

User = get_user_model

class ChatUser(models.Model):
    name= models.CharField('닉네임',max_length=10)
    createdAt = models.DateTimeField('생성일', auto_now_add=True)
    

class Chat(models.Model):
    text = models.TextField('문자내용', null=False,blank=False)
    createdAt = models.DateTimeField('작성시간', auto_now_add=True)
    writer = models.ForeignKey(to=ChatUser, on_delete=models.CASCADE)
