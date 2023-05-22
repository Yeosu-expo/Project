from django.contrib import admin
from .models import Post, Comment

class CommentInline(admin.TabularInline):
    model = Comment
    extra = 5
    min_num =3
    max_num=5

@admin.register(Post)
class PostModelAdmin(admin.ModelAdmin):
    list_display=('id',
        'image',
        'content',
        'created_at',
        'view_count',
        'writer'
    )
    # list_editable = ('content',)
    list_filter = ('created_at', )
    search_fields = ('id', 'writer__username') # search는 한 명을 특정하기 위한 기능
    search_help_text = '게시판 번호, 작성자 검색이 가능합니다.'
    inlines=[CommentInline]
    readonly_fields = ('created_at',)

    actions=['make_published']

    def make_published(modeladmin, requese, queryset):
        for item in queryset:
            item.content='운영 규정 위반으로 인한 게시글 삭제 처리'
            item.save()

# admin.site.register(Comment)
