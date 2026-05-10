import { Shield, ShieldAlert, ShieldCheck } from "lucide-react";
import type { AirspaceInfo } from "@/hooks/useAirspace";

interface AirspaceStatusProps {
  airspaces: AirspaceInfo[];
}

const RESTRICTED_TYPES = ["Restricted", "Prohibited", "Danger", "TFR"];
const CONTROLLED_TYPES = ["CTR", "TMA", "ATZ", "CTA", "TMZ", "RMZ"];

function getZoneColor(type: string): string {
  if (RESTRICTED_TYPES.includes(type)) return "text-red-400";
  if (CONTROLLED_TYPES.includes(type)) return "text-amber-400";
  return "text-blue-400";
}

function getZoneBg(type: string): string {
  if (RESTRICTED_TYPES.includes(type)) return "bg-red-500/10 border-red-500/20";
  if (CONTROLLED_TYPES.includes(type)) return "bg-amber-500/10 border-amber-500/20";
  return "bg-blue-500/10 border-blue-500/20";
}

export function AirspaceStatus({ airspaces }: AirspaceStatusProps) {
  const hasRestricted = airspaces.some((a) => RESTRICTED_TYPES.includes(a.type));
  const hasControlled = airspaces.some((a) => CONTROLLED_TYPES.includes(a.type));

  return (
    <div className="flex flex-col gap-2">
      {/* Overall status */}
      <div className="flex items-center gap-2">
        {hasRestricted ? (
          <ShieldAlert className="h-4 w-4 text-red-400" />
        ) : hasControlled ? (
          <Shield className="h-4 w-4 text-amber-400" />
        ) : (
          <ShieldCheck className="h-4 w-4 text-green-400" />
        )}
        <span
          className={`text-xs font-medium ${
            hasRestricted
              ? "text-red-400"
              : hasControlled
                ? "text-amber-400"
                : "text-green-400"
          }`}
        >
          {hasRestricted
            ? "RESTRICTED AIRSPACE"
            : hasControlled
              ? "CONTROLLED AIRSPACE"
              : airspaces.length > 0
                ? "AIRSPACE OK"
                : "NO AIRSPACE DATA"}
        </span>
      </div>

      {/* Airspace list */}
      {airspaces.map((a, i) => (
        <div
          key={i}
          className={`rounded border px-2 py-1.5 ${getZoneBg(a.type)}`}
        >
          <div className="flex items-center justify-between">
            <span className={`text-[11px] font-semibold ${getZoneColor(a.type)}`}>
              {a.type}
            </span>
            <span className="text-[10px] text-[hsl(var(--muted-foreground))]">
              Class {a.icaoClass}
            </span>
          </div>
          <div className="text-[11px] text-[hsl(var(--foreground))] mt-0.5">
            {a.name}
          </div>
          <div className="text-[10px] text-[hsl(var(--muted-foreground))] mt-0.5">
            {a.lowerLimit} → {a.upperLimit}
          </div>
        </div>
      ))}
    </div>
  );
}
