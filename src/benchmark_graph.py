import matplotlib.pyplot as plt
import re
import sys
import os

# 파일 경로 설정
if len(sys.argv) > 1:
    result_file = sys.argv[1]
else:
    result_file = '../result.txt'

# 현재 디렉토리에 파일이 없으면 업로드 디렉토리에서 찾기
if not os.path.exists(result_file):
    result_file = '/mnt/user-data/uploads/result.txt'

# 파일에서 데이터 읽기
try:
    with open(result_file, 'r', encoding='utf-8') as f:
        data = f.read()
    print(f"✓ 파일 읽음: {result_file}")
except FileNotFoundError:
    print(f"❌ Error: 파일을 찾을 수 없습니다: {result_file}")
    sys.exit(1)

languages = []
times = []

# 정규식으로 데이터 추출
pattern = r'(?:Building|Run)\s+(.+?)\.\.\.\n.*?Time:\s+([\d.]+)\s+ms'
matches = re.findall(pattern, data, re.DOTALL)

for language, time_str in matches:
    languages.append(language)
    times.append(float(time_str))

# 시간 순으로 정렬
sorted_data = sorted(zip(languages, times), key=lambda x: x[1])
languages_sorted = [item[0] for item in sorted_data]
times_sorted = [item[1] for item in sorted_data]

# 그래프 생성
fig, ax = plt.subplots(figsize=(10, 6))

# 바 그래프
colors = ['#2ecc71' if i == 0 else '#e74c3c' if i == len(times_sorted) - 1 else '#3498db' 
          for i in range(len(times_sorted))]
bars = ax.bar(languages_sorted, times_sorted, color=colors, edgecolor='black', linewidth=1.5)

# 값을 바 위에 표시
for bar, time in zip(bars, times_sorted):
    height = bar.get_height()
    ax.text(bar.get_x() + bar.get_width()/2., height,
            f'{time:,.2f} ms',
            ha='center', va='bottom', fontsize=10, fontweight='bold')

# 레이아웃 설정
ax.set_ylabel('Time (ms)', fontsize=12, fontweight='bold')
ax.set_xlabel('Language', fontsize=12, fontweight='bold')
ax.set_title('Benchmark Results - File Reading Performance\n(100,000,000 lines)', 
             fontsize=14, fontweight='bold')
ax.grid(axis='y', alpha=0.3, linestyle='--')
ax.set_ylim(0, max(times_sorted) * 1.15)

plt.tight_layout()
plt.savefig('../benchmark_bar_chart.png', dpi=300, bbox_inches='tight')
print("✓ 바 그래프 저장: benchmark_bar_chart.png")

# 추가: 라인 그래프
fig, ax = plt.subplots(figsize=(10, 6))
ax.plot(languages_sorted, times_sorted, marker='o', markersize=10, 
        linewidth=2.5, color='#3498db', markerfacecolor='#2ecc71', 
        markeredgecolor='black', markeredgewidth=2)

# 값을 점 위에 표시
for i, (lang, time) in enumerate(zip(languages_sorted, times_sorted)):
    ax.text(i, time + 150, f'{time:,.2f} ms', ha='center', fontsize=10, fontweight='bold')

ax.set_ylabel('Time (ms)', fontsize=12, fontweight='bold')
ax.set_xlabel('Language', fontsize=12, fontweight='bold')
ax.set_title('Benchmark Results - File Reading Performance\n(100,000,000 lines)', 
             fontsize=14, fontweight='bold')
ax.grid(True, alpha=0.3, linestyle='--')
ax.set_ylim(min(times_sorted) - 500, max(times_sorted) + 500)

plt.tight_layout()
plt.savefig('../benchmark_line_chart.png', dpi=300, bbox_inches='tight')
print("✓ 라인 그래프 저장: benchmark_line_chart.png")

# 통계 출력
print("\n" + "="*50)
print("BENCHMARK STATISTICS")
print("="*50)
print(f"{'Language':<12} {'Time (ms)':<15} {'vs Fastest':<15}")
print("-"*50)
fastest_time = min(times_sorted)
for lang, time in zip(languages_sorted, times_sorted):
    ratio = time / fastest_time
    print(f"{lang:<12} {time:>10,.2f} ms   {ratio:>8.2f}x")
print("="*50)
print(f"Fastest: {languages_sorted[0]} ({times_sorted[0]:,.2f} ms)")
print(f"Slowest: {languages_sorted[-1]} ({times_sorted[-1]:,.2f} ms)")
print(f"Average: {sum(times_sorted)/len(times_sorted):,.2f} ms")
print("="*50)