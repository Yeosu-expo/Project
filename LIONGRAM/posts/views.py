from django.shortcuts import render, redirect, get_object_or_404
from django.http import HttpResponse, Http404
from django.views.generic import ListView
from .models import Post
from django.contrib.auth.decorators import login_required

from .forms import PostBaseForm, PostCreateForm, PostDetailForm

def post_create_form_view(request):
    if request.method == 'GET':
        form = PostCreateForm()
        return render(request, 'posts/post_form2.html', {'form':form })
    else:
        form = PostBaseForm(request.POST, request.FILES)
        
        if form.is_valid():
            Post.objects.create(
                image=form.cleaned_data['image'],
                content=form.cleaned_data['content'],
                writer=request.user,
            )
        else:
            return redirect('posts:post-create')
        return redirect('index')

def index(request):
    post_list = Post.objects.all()
    context={
        'post_list' : post_list
    }
    return render(request, 'index.html', context)

def post_list_view(request):
    #post_list = Post.objects.all()
    post_list = Post.objects.filter(writer=request.user)
    context={
        'post_list' : post_list
    }
    return render(request, 'posts/post_list.html', context)

@login_required
def post_create_view(request):
    if request.method == 'GET':
        return render(request, 'posts/post_form.html')
    else:
        image =  request.FILES.get('image')
        content = request.POST.get('content')
        print(image)
        print(content)
        Post.objects.create(
            image=image,
            content=content,
            writer=request.user,
        )
        return redirect('index')
    
@login_required
def post_update_view(request, id):
    post = get_object_or_404(Post, id=id)

    if request.method == 'GET':
        context={ 'post':post }
        return render(request, 'posts/post_form.html', context)
    elif request.method == 'POST':
        new_image =  request.FILES.get('image')
        content = request.POST.get('content')

        if new_image:
            post.image.delete()
            post.image = new_image
        post.content = content
        post.save()
        return redirect('posts:post-detail', post.id)

@login_required
def post_delete_view(request, id):
    post = get_object_or_404(Post, id=id)

    if request.user != post.writer:
        return Http404('잘못된 접근입니다.')

    if request.method == 'GET':
        context={ 'post':post }
        return render(request, 'posts/post_confirm_delete.html', context)
    else:
        post.delete()
        return redirect('index')

def post_detail_view(request, id):
    try:
        post = Post.objects.get(id=id)
    except Post.DoesNotExist:
        return redirect('index')
    context={
        'post': post,
        'form': PostDetailForm
    }
    return render(request, 'posts/post_detail.html', context)

def url_view(request):
    return HttpResponse('<h1>url_view')

def url_parameter_view(request, username):
    print('url_parameter_view()')
    print(username)
    print(request.GET)
    return HttpResponse()

def function_view(request):
    print(f'request.method: {request.method}')
    print(f'request.GET: {request.GET}')
    print(f'request.POST: {request.POST}')
    return render(request, 'view2.html')

class class_view(ListView):
    model = Post
    template_name = 'cbv_views.html'