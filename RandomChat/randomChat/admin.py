from django.contrib import admin
from .models import Chat, ChatUser

@admin.register(ChatUser)
@admin.register(Chat)
class PostModelUser(admin.ModelAdmin):
    pass
