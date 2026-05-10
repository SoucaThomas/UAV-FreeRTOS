import { useState, useEffect, useRef } from "react";

const OPENAIP_API_KEY = import.meta.env.VITE_OPENAIP_API_KEY as string;

const AIRSPACE_TYPES: Record<number, string> = {
  0: "Other",
  1: "Restricted",
  2: "Danger",
  3: "Prohibited",
  4: "CTR",
  5: "TMZ",
  6: "RMZ",
  7: "TMA",
  8: "TRA",
  9: "TSA",
  10: "FIR",
  11: "UIR",
  12: "ADIZ",
  13: "ATZ",
  14: "MATZ",
  15: "Airway",
  16: "MTR",
  17: "Alert Area",
  18: "Warning Area",
  19: "Protected",
  20: "HTZ",
  21: "Gliding Sector",
  22: "TRP",
  23: "TIZ",
  24: "TIA",
  25: "MTA",
  26: "CTA",
  27: "ACC Sector",
  28: "Aerial Sporting",
  29: "Low Altitude Overflight",
  30: "MRT",
  31: "TFR",
  32: "VFR Sector",
  33: "FIS Sector",
};

const ICAO_CLASSES: Record<number, string> = {
  0: "A",
  1: "B",
  2: "C",
  3: "D",
  4: "E",
  5: "F",
  6: "G",
  7: "SUA",
  8: "Unclassified",
};

export interface AirspaceInfo {
  name: string;
  type: string;
  icaoClass: string;
  upperLimit: string;
  lowerLimit: string;
}

function formatLimit(limit: { value: number; unit: number; referenceDatum: number }): string {
  const units = ["ft", "m", "FL", "ft", "m", "ft", "FL"];
  const datums = ["GND", "MSL", "STD"];
  const unit = units[limit.unit] ?? "";
  const datum = datums[limit.referenceDatum] ?? "";
  if (limit.value === 0 && limit.referenceDatum === 0) return "GND";
  if (limit.unit === 6) return `FL${limit.value}`;
  return `${limit.value} ${unit} ${datum}`.trim();
}

export function useAirspace(position: [number, number] | null): AirspaceInfo[] {
  const [airspaces, setAirspaces] = useState<AirspaceInfo[]>([]);
  const lastQueryRef = useRef<string>("");

  useEffect(() => {
    if (!position || !OPENAIP_API_KEY) return;

    // Round position to reduce API calls (only query when moved significantly)
    const key = `${position[0].toFixed(3)},${position[1].toFixed(3)}`;
    if (key === lastQueryRef.current) return;
    lastQueryRef.current = key;

    const controller = new AbortController();

    fetch(
      `https://api.core.openaip.net/api/airspaces?apiKey=${OPENAIP_API_KEY}&pos=${position[1].toFixed(5)},${position[0].toFixed(5)}&dist=20000&limit=10`,
      { signal: controller.signal }
    )
      .then((res) => res.json())
      .then((data) => {
        const items = data.items ?? [];
        setAirspaces(
          items
            .filter((a: any) => a.type !== 10 && a.type !== 11 && a.type !== 33) // Skip FIR/UIR/FIS
            .sort((a: any, b: any) => {
              // Prioritize: Prohibited > Restricted > Danger > CTR > TMA > rest
              const priority: Record<number, number> = { 3: 0, 1: 1, 2: 2, 4: 3, 7: 4 };
              return (priority[a.type] ?? 99) - (priority[b.type] ?? 99);
            })
            .slice(0, 3)
            .map((a: any) => ({
              name: a.name ?? "Unknown",
              type: AIRSPACE_TYPES[a.type] ?? `Type ${a.type}`,
              icaoClass: ICAO_CLASSES[a.icaoClass] ?? "?",
              upperLimit: a.upperLimit ? formatLimit(a.upperLimit) : "?",
              lowerLimit: a.lowerLimit ? formatLimit(a.lowerLimit) : "?",
            }))
        );
      })
      .catch(() => {
        // Silently fail on network errors
      });

    return () => controller.abort();
  }, [position]);

  return airspaces;
}
