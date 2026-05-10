import { useEffect, useState } from "react";
import {
  Map,
  MapControls,
  MapMarker,
  MarkerContent,
  MapRoute,
} from "@/components/ui/map";
import { AirspaceLayer } from "@/components/AirspaceLayer";
import type { SimulatedFlightState } from "@/hooks/useSimulatedFlight";

interface MapViewProps {
  flight?: SimulatedFlightState | null;
  showAirspace?: boolean;
  airspaceOpacity?: number;
  mapStyle?: string;
}

export function MapView({ flight, showAirspace = false, airspaceOpacity = 0.35, mapStyle }: MapViewProps) {
  const [userLocation, setUserLocation] = useState<[number, number] | null>(
    null
  );
  const [status, setStatus] = useState<string>("Getting location...");

  useEffect(() => {
    if (!navigator.geolocation) {
      setStatus("");
      setUserLocation([23.6, 46.77]);
      return;
    }

    navigator.geolocation.getCurrentPosition(
      (pos) => {
        setUserLocation([pos.coords.longitude, pos.coords.latitude]);
        setStatus("");
      },
      () => {
        setUserLocation([23.6, 46.77]);
        setStatus("");
      },
      { timeout: 5000 }
    );
  }, []);

  if (status || !userLocation) {
    return (
      <div className="flex h-full w-full items-center justify-center bg-[hsl(224,71.4%,4.1%)]">
        <span className="text-sm text-[hsl(var(--muted-foreground))]">
          {status}
        </span>
      </div>
    );
  }

  return (
    <Map
      key={mapStyle}
      center={userLocation}
      zoom={15}
      className="h-full w-full"
      theme="dark"
      {...(mapStyle ? { styles: { dark: mapStyle, light: mapStyle } } : {})}
    >
      <AirspaceLayer visible={showAirspace} opacity={airspaceOpacity} />
      <MapControls
        position="top-right"
        showZoom
        showLocate
        showCompass
        onLocate={(coords) =>
          setUserLocation([coords.longitude, coords.latitude])
        }
      />

      {/* Ground station marker */}
      <MapMarker longitude={userLocation[0]} latitude={userLocation[1]}>
        <MarkerContent>
          <div className="relative flex items-center justify-center">
            <div className="absolute h-8 w-8 rounded-full bg-blue-500/20 animate-ping" />
            <div className="flex h-4 w-4 items-center justify-center rounded-full border-2 border-white bg-blue-500 shadow-lg" />
          </div>
        </MarkerContent>
      </MapMarker>

      {/* UAV trail */}
      {flight && flight.trail.length >= 2 && (
        <MapRoute
          coordinates={flight.trail}
          color="#f59e0b"
          width={2}
          opacity={0.6}
          interactive={false}
        />
      )}

      {/* UAV marker */}
      {flight && (
        <MapMarker
          longitude={flight.position[0]}
          latitude={flight.position[1]}
          rotation={flight.heading}
          rotationAlignment="map"
        >
          <MarkerContent>
            <svg width="32" height="32" viewBox="0 0 32 32">
              {/* Shadow */}
              <polygon
                points="16,4 10,24 16,20 22,24"
                fill="black"
                opacity="0.3"
                transform="translate(1,1)"
              />
              {/* Plane body */}
              <polygon
                points="16,4 10,24 16,20 22,24"
                fill="#f59e0b"
                stroke="#fff"
                strokeWidth="1"
              />
            </svg>
          </MarkerContent>
        </MapMarker>
      )}
    </Map>
  );
}

export { type MapViewProps };
