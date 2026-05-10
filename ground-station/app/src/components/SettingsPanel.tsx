export const MAP_STYLES = {
  "Dark Matter": "https://basemaps.cartocdn.com/gl/dark-matter-gl-style/style.json",
  "Dark (No Labels)": "https://basemaps.cartocdn.com/gl/dark-matter-nolabels-gl-style/style.json",
  "Positron (Light)": "https://basemaps.cartocdn.com/gl/positron-gl-style/style.json",
  "Voyager": "https://basemaps.cartocdn.com/gl/voyager-gl-style/style.json",
  "Voyager (No Labels)": "https://basemaps.cartocdn.com/gl/voyager-nolabels-gl-style/style.json",
  "OSM Liberty": "https://tiles.openfreemap.org/styles/liberty",
  "OSM Bright": "https://tiles.openfreemap.org/styles/bright",
} as const;

export type MapStyleName = keyof typeof MAP_STYLES;

interface SettingsPanelProps {
  mapStyle: MapStyleName;
  onMapStyleChange: (style: MapStyleName) => void;
  airspaceOpacity: number;
  onAirspaceOpacityChange: (opacity: number) => void;
}

export function SettingsPanel({
  mapStyle,
  onMapStyleChange,
  airspaceOpacity,
  onAirspaceOpacityChange,
}: SettingsPanelProps) {
  return (
    <div className="flex flex-col gap-4">
      {/* Map Style */}
      <div>
        <label className="text-[11px] font-medium text-[hsl(var(--muted-foreground))] uppercase tracking-wide">
          Map Style
        </label>
        <div className="mt-2 grid grid-cols-1 gap-1.5">
          {Object.keys(MAP_STYLES).map((name) => (
            <button
              key={name}
              onClick={() => onMapStyleChange(name as MapStyleName)}
              className={`rounded-md px-3 py-1.5 text-left text-xs transition-colors ${
                mapStyle === name
                  ? "bg-blue-500/20 text-blue-400 border border-blue-500/30"
                  : "text-[hsl(var(--foreground))] hover:bg-white/10 border border-transparent"
              }`}
            >
              {name}
            </button>
          ))}
        </div>
      </div>

      <div className="h-px bg-white/10" />

      {/* Airspace Opacity */}
      <div>
        <label className="text-[11px] font-medium text-[hsl(var(--muted-foreground))] uppercase tracking-wide">
          Airspace Overlay Opacity
        </label>
        <div className="mt-2 flex items-center gap-3">
          <input
            type="range"
            min={0}
            max={100}
            value={airspaceOpacity * 100}
            onChange={(e) => onAirspaceOpacityChange(Number(e.target.value) / 100)}
            className="w-full h-1.5 bg-white/10 rounded-full appearance-none cursor-pointer
              [&::-webkit-slider-thumb]:appearance-none [&::-webkit-slider-thumb]:w-3 [&::-webkit-slider-thumb]:h-3
              [&::-webkit-slider-thumb]:rounded-full [&::-webkit-slider-thumb]:bg-blue-400"
          />
          <span className="text-[11px] font-mono text-[hsl(var(--foreground))] w-8 text-right">
            {Math.round(airspaceOpacity * 100)}%
          </span>
        </div>
      </div>
    </div>
  );
}
