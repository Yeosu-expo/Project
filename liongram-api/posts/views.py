from django.shortcuts import render
from rest_framework.response import Response
from rest_framework.decorators import api_view
from rest_framework.views import APIView

@api_view()
def calculator(request):
    # 데이터 삽입
    num1 = request.GET.get('num1', 0)
    num2 = request.GET.get('num2', 0)
    operators = request.GET.get('operators')
    # 계산
    if operators == '+':
        result = int(num1)+int(num2)
    elif operators == '-':
        result = int(num1)-int(num2)
    elif operators == '*':
        result = int(num1)*int(num2)
    elif operators == '/':
        result = int(num1)/int(num2)
    else:
        result = 0

    data={
        'type':'FBV',
        'result':result
    }
    # 응답
    return Response(data=data)

class CalculatorAPIView(APIView):
    def get(self, request):
        # 데이터 삽입
        num1 = request.GET.get('num1', 0)
        num2 = request.GET.get('num2', 0)
        operators = request.GET.get('operators')
        # 계산
        if operators == '+':
            result = int(num1)+int(num2)
        elif operators == '-':
            result = int(num1)-int(num2)
        elif operators == '*':
            result = int(num1)*int(num2)
        elif operators == '/':
            result = int(num1)/int(num2)
        else:
            result = 0

        data={
            'type':'CBV',
            'result':result
        }
        # 응답
        return Response(data=data)
    
    def post(self, request):
        data={
            'type':'CBV',
            'method': 'POST',
            'result':0
        }
        # 응답
        return Response(data=data)