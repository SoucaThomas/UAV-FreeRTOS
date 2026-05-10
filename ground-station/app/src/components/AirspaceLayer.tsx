import { useEffect, useState } from "react";
import { useMap } from "@/components/ui/map";

const OPENAIP_API_KEY = import.meta.env.VITE_OPENAIP_API_KEY as string;

const SOURCE_ID = "openaip-airspace";
const LAYER_ID = "openaip-airspace-layer";

interface AirspaceLayerProps {
  visible?: boolean;
  opacity?: number;
}

export function AirspaceLayer({ visible = true, opacity = 0.35 }: AirspaceLayerProps) {
  const { map, isLoaded } = useMap();
  const [added, setAdded] = useState(false);

  useEffect(() => {
    if (!map || !isLoaded || !OPENAIP_API_KEY) return;

    if (!map.getSource(SOURCE_ID)) {
      map.addSource(SOURCE_ID, {
        type: "raster",
        tiles: [
          `https://api.tiles.openaip.net/api/data/openaip/{z}/{x}/{y}.png?apiKey=${OPENAIP_API_KEY}`,
        ],
        tileSize: 256,
      });
    }

    if (!map.getLayer(LAYER_ID)) {
      map.addLayer({
        id: LAYER_ID,
        type: "raster",
        source: SOURCE_ID,
        paint: {
          "raster-opacity": opacity,
        },
      });
    }

    setAdded(true);

    return () => {
      try {
        if (map.getLayer(LAYER_ID)) map.removeLayer(LAYER_ID);
        if (map.getSource(SOURCE_ID)) map.removeSource(SOURCE_ID);
      } catch {
        // map may already be removed
      }
      setAdded(false);
    };
  }, [map, isLoaded]);

  // Toggle visibility and opacity
  useEffect(() => {
    if (!map || !added) return;
    try {
      map.setLayoutProperty(
        LAYER_ID,
        "visibility",
        visible ? "visible" : "none"
      );
      map.setPaintProperty(LAYER_ID, "raster-opacity", opacity);
    } catch {
      // layer may not exist yet
    }
  }, [map, added, visible, opacity]);

  if (!OPENAIP_API_KEY) {
    console.warn("VITE_OPENAIP_API_KEY not set — airspace layer disabled");
  }

  return null;
}
