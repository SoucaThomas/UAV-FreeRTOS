interface ThrottleBarProps {
  value: number; // 0 to 1
}

export function ThrottleBar({ value }: ThrottleBarProps) {
  const pct = Math.round(value * 100);
  const barHeight = 80;
  const fillHeight = value * barHeight;

  const color =
    pct > 80 ? "#ef4444" : pct > 50 ? "#f59e0b" : "#22c55e";

  return (
    <div className="flex flex-col items-center gap-1">
      <span className="text-[10px] text-[hsl(var(--muted-foreground))] font-medium">
        THR
      </span>
      <svg
        width={24}
        height={barHeight + 4}
        viewBox={`0 0 24 ${barHeight + 4}`}
        className="select-none"
      >
        {/* Background */}
        <rect
          x={2}
          y={2}
          width={20}
          height={barHeight}
          rx={3}
          fill="hsl(215, 27.9%, 10%)"
          stroke="hsl(215, 27.9%, 20%)"
          strokeWidth={1}
        />
        {/* Fill (from bottom) */}
        <rect
          x={4}
          y={barHeight + 2 - fillHeight}
          width={16}
          height={fillHeight}
          rx={2}
          fill={color}
          opacity={0.8}
        />
        {/* Tick marks */}
        {[0, 25, 50, 75, 100].map((tick) => {
          const y = 2 + barHeight - (tick / 100) * barHeight;
          return (
            <line
              key={tick}
              x1={0}
              y1={y}
              x2={4}
              y2={y}
              stroke="hsl(215, 27.9%, 30%)"
              strokeWidth={0.75}
            />
          );
        })}
      </svg>
      <span className="text-[10px] font-medium text-[hsl(var(--foreground))] tabular-nums">
        {pct}%
      </span>
    </div>
  );
}
