interface StickDisplayProps {
  label: string;
  x: number; // -1 to 1
  y: number; // -1 to 1
  size?: number;
}

export function StickDisplay({ label, x, y, size = 80 }: StickDisplayProps) {
  const cx = size / 2;
  const cy = size / 2;
  const range = size / 2 - 8;

  // Map -1..1 to pixel position
  const dotX = cx + x * range;
  const dotY = cy + y * range;

  return (
    <div className="flex flex-col items-center gap-1">
      <span className="text-[10px] text-[hsl(var(--muted-foreground))] font-medium">
        {label}
      </span>
      <svg
        width={size}
        height={size}
        viewBox={`0 0 ${size} ${size}`}
        className="select-none"
      >
        {/* Background circle */}
        <circle
          cx={cx}
          cy={cy}
          r={range + 3}
          fill="hsl(215, 27.9%, 10%)"
          stroke="hsl(215, 27.9%, 20%)"
          strokeWidth={1}
        />

        {/* Cross hairs */}
        <line
          x1={cx}
          y1={cy - range}
          x2={cx}
          y2={cy + range}
          stroke="hsl(215, 27.9%, 20%)"
          strokeWidth={0.75}
        />
        <line
          x1={cx - range}
          y1={cy}
          x2={cx + range}
          y2={cy}
          stroke="hsl(215, 27.9%, 20%)"
          strokeWidth={0.75}
        />

        {/* Inner guide */}
        <circle
          cx={cx}
          cy={cy}
          r={range * 0.5}
          fill="none"
          stroke="hsl(215, 27.9%, 16%)"
          strokeWidth={0.5}
        />

        {/* Stick position dot */}
        <circle cx={dotX} cy={dotY} r={6} fill="#3b82f6" opacity={0.9} />
        <circle cx={dotX} cy={dotY} r={3} fill="#60a5fa" />
      </svg>
      <div className="flex gap-2 text-[10px] text-[hsl(var(--muted-foreground))] tabular-nums">
        <span>{x.toFixed(2)}</span>
        <span>{y.toFixed(2)}</span>
      </div>
    </div>
  );
}
