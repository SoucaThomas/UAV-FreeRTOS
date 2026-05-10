interface AttitudeIndicatorProps {
  pitch: number; // degrees, -90 to 90
  roll: number; // degrees, -180 to 180
  size?: number;
}

export function AttitudeIndicator({
  pitch,
  roll,
  size = 120,
}: AttitudeIndicatorProps) {
  const cx = size / 2;
  const cy = size / 2;
  const r = size / 2 - 3;

  // Pitch offset: map pitch degrees to pixels (scale factor)
  const pitchPixelsPerDeg = r / 45;
  const pitchOffset = pitch * pitchPixelsPerDeg;

  const clipId = `attitude-clip-${size}`;

  return (
    <div className="flex flex-col items-center gap-1">
      <svg
        width={size}
        height={size}
        viewBox={`0 0 ${size} ${size}`}
        className="select-none"
      >
        <defs>
          <clipPath id={clipId}>
            <circle cx={cx} cy={cy} r={r} />
          </clipPath>
        </defs>

        {/* Background group, rotated by roll and shifted by pitch */}
        <g clipPath={`url(#${clipId})`}>
          <g transform={`rotate(${-roll}, ${cx}, ${cy})`}>
            {/* Sky */}
            <rect
              x={0}
              y={-size}
              width={size}
              height={size + cy + pitchOffset}
              fill="#2563eb"
            />
            {/* Ground */}
            <rect
              x={0}
              y={cy + pitchOffset}
              width={size}
              height={size + size}
              fill="#854d0e"
            />
            {/* Horizon line */}
            <line
              x1={0}
              y1={cy + pitchOffset}
              x2={size}
              y2={cy + pitchOffset}
              stroke="white"
              strokeWidth={1}
              opacity={0.8}
            />

            {/* Pitch markings */}
            {[-20, -10, 10, 20].map((deg) => {
              const y = cy + pitchOffset - deg * pitchPixelsPerDeg;
              const w = Math.abs(deg) === 20 ? 12 : 18;
              return (
                <line
                  key={deg}
                  x1={cx - w}
                  y1={y}
                  x2={cx + w}
                  y2={y}
                  stroke="white"
                  strokeWidth={0.75}
                  opacity={0.5}
                />
              );
            })}
          </g>
        </g>

        {/* Fixed aircraft reference */}
        <g stroke="#f59e0b" strokeWidth={2} strokeLinecap="round">
          <line x1={cx - 25} y1={cy} x2={cx - 8} y2={cy} />
          <line x1={cx + 8} y1={cy} x2={cx + 25} y2={cy} />
          <line x1={cx - 8} y1={cy} x2={cx - 8} y2={cy + 5} />
          <line x1={cx + 8} y1={cy} x2={cx + 8} y2={cy + 5} />
        </g>
        {/* Center dot */}
        <circle cx={cx} cy={cy} r={2} fill="#f59e0b" />

        {/* Outer ring */}
        <circle
          cx={cx}
          cy={cy}
          r={r}
          fill="none"
          stroke="hsl(215, 27.9%, 16.9%)"
          strokeWidth={2}
        />

        {/* Roll indicator marks */}
        {[-45, -30, -20, -10, 0, 10, 20, 30, 45].map((deg) => {
          const angle = (deg - 90) * (Math.PI / 180);
          const innerR = r - 5;
          const outerR = r;
          return (
            <line
              key={deg}
              x1={cx + innerR * Math.cos(angle)}
              y1={cy + innerR * Math.sin(angle)}
              x2={cx + outerR * Math.cos(angle)}
              y2={cy + outerR * Math.sin(angle)}
              stroke="white"
              strokeWidth={deg === 0 ? 1.5 : 0.75}
              opacity={0.5}
            />
          );
        })}

        {/* Roll pointer */}
        <polygon
          points={`${cx},${cy - r + 6} ${cx - 3},${cy - r + 12} ${cx + 3},${cy - r + 12}`}
          fill="#f59e0b"
        />
      </svg>
      <div className="flex gap-4 text-[10px] text-[hsl(var(--muted-foreground))]">
        <span>P: {pitch.toFixed(1)}</span>
        <span>R: {roll.toFixed(1)}</span>
      </div>
    </div>
  );
}
