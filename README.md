# Malloc-lab

## 구현 결과
|List Type|Allocation|Utilization(자원 활용도)|throughput(시간당 처리율)|Sum/100|Rank|
|:--:|:--:|:--:|:--:|:--:|:--:|
|Implicit|First Fit|44|8|53|6️⃣|
|Implicit|Next Fit|44|40|84|🥇|
|Implicit|Best Fit|45|8|53|5️⃣|
|Implicit|Worst Fit|41|8|49|7️⃣|
|Explicit|First Fit|42|40|82|🥈|
|Explicit|Next Fit|42|40|82|🥈|
|Explicit|Best Fit|45|26|71|🥉|
|Explicit|Worst Fit|41|25|66|4️⃣|

(청크사이즈를 `1<<12`가 아닌 `4100`~`4600`으로 하면 점수가 1점 오르나 과적합으로 생각되어 결과에 반영하지는 않음)

## 학습 목표
1. 동적 메모리 할당기(malloc, realloc, free)를 직접 개발한다.
2. 메모리, 포인터 개념에 익숙해진다.
   
## 학습 일정
(크래프톤 정글 3기 week. 5) 2023/11/09 ~ 16

## 학습 결과
1. 동적 할당기 개념 이해함.
2. 묵시적 해제 리스트, 명시적 해제 리스트 개념 이해함.
3. 4가지의 메모리 할당 전략 이해함.
4. 묵시적 해제 리스트, 명시적 해제 리스트로 각각 4가지 할당 전략 구현  
   (explicit에서 next fit방식은 사실상 유효한 전략은 아님)

## 참고 자료
[CMU 카네기멜론대학](http://csapp.cs.cmu.edu/3e/malloclab.pdf)

## 참고 포스팅
[[크래프톤 정글 3기] 11/15(수) TIL](https://velog.io/@classbinu/%ED%81%AC%EB%9E%98%ED%94%84%ED%86%A4-%EC%A0%95%EA%B8%80-3%EA%B8%B0-1115%EC%88%98-TIL)
