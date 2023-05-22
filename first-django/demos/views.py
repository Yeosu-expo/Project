from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.
def calculator(request):
    #return HttpResponse('계산기 기능 구현 시작입니다.') # 템플릿을 사용하지 않고 응답을 할 수 있음
    print(f'request={request}')

    # 1. 데이터 확인
    num1 = request.GET.get('num1')
    num2 = request.GET.get('num2')
    operators = request.GET.get('operators')

    # 2. 계산
    if operators == '+':
        result = int(num1) + int(num2)
    elif operators == '-':
        result = int(num1) - int(num2)
    elif operators == '*':
        result = int(num1) * int(num2)
    elif operators == '/':
        result = int(num1) / int(num2)
    else:
        result = 0



    # 3. 응답

    return render(request, 'calculator.html', {'result': result})