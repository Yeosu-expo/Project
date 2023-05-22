from django.shortcuts import render
from django.http import HttpResponse
from .models import ChatUser, Chat

#def getUser(request):
    # username=request.GET.get('username')
    #ChatUser.objects.create(name=username)
    
    
def chat(request):
    chatInput=request.GET.get('chatInput')
    username=request.GET.get('username')
    if request.method == 'POST':
        Chat.objects.create(text=request.POST['chatInput'])
        ChatUser.objects.create(name=request.POST['username'])

    return render(request, 'randomChat.html', {'chatInput':chatInput, 'username':username})