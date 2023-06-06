from django.shortcuts import render, redirect
from .forms import UserCreateForm, SignUpFrom
from django.contrib.auth.forms import UserCreationForm, AuthenticationForm
from django.contrib.auth import login, logout

def signup_view(request):
    if request.method == 'GET':
        form=SignUpFrom
        context = {'form': form}
        return render(request, 'accounts/signup.html', context)
    else:
        form = SignUpFrom(request.POST)

        if form.is_valid():
            intstance = form.save()
            return redirect('index')
        else:
            return redirect('accounts:signup')
        

def login_view(request):
    if request.method == 'GET':
        return render(request, 'accounts/login.html', {'form': AuthenticationForm()})
    else:
        form = AuthenticationForm(request,data=request.POST)
        if form.is_valid():
            login(request, form.user_cache)
            return redirect('index')
        else:
            return render(request, 'accounts/login.html', {'form': form})
        
def logout_view(request):
    # 데이터 유효성 검사
    if request.user.is_authenticated:
        # 비지니스 로직 처리
        logout(request)

    # 응답
    return redirect('index')
