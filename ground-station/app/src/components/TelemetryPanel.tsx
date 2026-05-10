import { Badge } from "@/components/ui/badge";
import {
  Battery,
  Ruler,
  Timer,
  Radio,
} from "lucide-react";

export type FlightState =
  | "DISARMED"
  | "ARMED"
  | "TAKEOFF"
  | "FLIGHT"
  | "LANDING"
  | "ERROR";

interface TelemetryPanelProps {
  state: FlightState;
  batteryVoltage: number;
  batteryPercent: number;
  radioConnected: boolean;
  distanceHome?: number;
  ping?: number;
  rssi?: number;
}

function stateVariant(state: FlightState) {
  switch (state) {
    case "DISARMED":
      return "secondary" as const;
    case "ARMED":
      return "warning" as const;
    case "TAKEOFF":
    case "FLIGHT":
      return "success" as const;
    case "LANDING":
      return "warning" as const;
    case "ERROR":
      return "destructive" as const;
  }
}

function BatteryBar({ percent }: { percent: number }) {
  const color =
    percent > 50 ? "#22c55e" : percent > 20 ? "#f59e0b" : "#ef4444";

  return (
    <div className="h-1.5 w-full rounded-full bg-white/10">
      <div
        className="h-1.5 rounded-full transition-all duration-300"
        style={{ width: `${percent}%`, backgroundColor: color }}
      />
    </div>
  );
}

function TelemetryRow({
  icon: Icon,
  label,
  value,
}: {
  icon: React.ComponentType<{ className?: string }>;
  label: string;
  value: React.ReactNode;
}) {
  return (
    <div className="flex items-center justify-between">
      <div className="flex items-center gap-1.5">
        <Icon className="h-3 w-3 text-[hsl(var(--muted-foreground))]" />
        <span className="text-[11px] text-[hsl(var(--muted-foreground))]">
          {label}
        </span>
      </div>
      <span className="text-[11px] font-mono text-[hsl(var(--foreground))]">
        {value}
      </span>
    </div>
  );
}

export function TelemetryPanel({
  state,
  batteryVoltage,
  batteryPercent,
  radioConnected,
  distanceHome = 0,
  ping = 24,
  rssi = -62,
}: TelemetryPanelProps) {
  return (
    <div className="flex flex-col gap-3">
      {/* Flight State */}
      <div className="flex items-center justify-between">
        <span className="text-[11px] text-[hsl(var(--muted-foreground))]">
          State
        </span>
        <Badge variant={stateVariant(state)} className="text-[10px] px-2 py-0">
          {state}
        </Badge>
      </div>

      <div className="h-px bg-white/10" />

      {/* Battery */}
      <div className="space-y-1.5">
        <div className="flex items-center justify-between">
          <div className="flex items-center gap-1.5">
            <Battery className="h-3 w-3 text-[hsl(var(--muted-foreground))]" />
            <span className="text-[11px] text-[hsl(var(--muted-foreground))]">
              Battery
            </span>
          </div>
          <span className="text-[11px] font-mono text-[hsl(var(--foreground))]">
            {batteryVoltage.toFixed(2)}V / {batteryPercent}%
          </span>
        </div>
        <BatteryBar percent={batteryPercent} />
      </div>

      <div className="h-px bg-white/10" />

      {/* Metrics */}
      <div className="space-y-2">
        <TelemetryRow
          icon={Ruler}
          label="Distance"
          value={`${distanceHome.toFixed(0)} m`}
        />
        <TelemetryRow icon={Timer} label="Ping" value={`${ping} ms`} />
        <TelemetryRow
          icon={Radio}
          label="RSSI"
          value={`${rssi} dBm`}
        />
      </div>

      <div className="h-px bg-white/10" />

      {/* Link */}
      <div className="flex items-center justify-between">
        <span className="text-[11px] text-[hsl(var(--muted-foreground))]">
          Radio Link
        </span>
        <Badge
          variant={radioConnected ? "success" : "destructive"}
          className="text-[10px] px-2 py-0"
        >
          {radioConnected ? "Connected" : "Disconnected"}
        </Badge>
      </div>
    </div>
  );
}
