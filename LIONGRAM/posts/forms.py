from django import forms

from .models import Post


# class PostBaseForm(forms.Form):
#     CATEGORY_CHOICES = [
#         ('1', '일반'),
#         ('2', '계정')
#     ]
#     image = forms.ImageField(label='이미지')
#     content = forms.CharField(label='내용',widget=forms.Textarea, required=True)
#     #category = forms.ChoiceField(label='카테고리',choices=CATEGORY_CHOICES)

class PostBaseForm(forms.ModelForm):
    class Meta:
        model = Post
        fields = '__all__'

class PostCreateForm(PostBaseForm):
    class Meta(PostBaseForm.Meta):
        fields = ['image', 'content']

class PostDeleteForm(PostBaseForm):
    class Meta(PostBaseForm.Meta):
        fields = ['image', 'content']

class PostUpdateForm(PostBaseForm):
    class Meta(PostBaseForm.Meta):
        fields = ['image', 'content']

class PostDetailForm(PostBaseForm):
    pass