import api from "../lib/timestream";
import React from "react";
import useConfig from "./useConfig";
import { Auth } from "aws-amplify";
import { AreaClosed, Line, Bar } from "@vx/shape";
import { curveMonotoneX } from "@vx/curve";
import { GridRows, GridColumns } from "@vx/grid";
import { scaleTime, scaleLinear } from "@vx/scale";
import { withTooltip, Tooltip, defaultStyles } from "@vx/tooltip";
import { WithTooltipProvidedProps } from "@vx/tooltip/lib/enhancers/withTooltip";
import { localPoint } from "@vx/event";
import { LinearGradient } from "@vx/gradient";
import { max, extent, bisector } from "d3-array";
import { timeFormat } from "d3-time-format";

export const background = "#3b6978";
export const background2 = "#204051";
export const accentColor = "#edffea";
export const accentColorDark = "#75daad";
const tooltipStyles = {
  ...defaultStyles,
  background,
  border: "1px solid white",
  color: "white",
};

// util
const formatDate = timeFormat("%b %d, '%y %H:%M");

// accessors
const getDate = (d: Datum) => new Date(d.time);
const getValue = (d: Datum) => d.co2;
const bisectDate = bisector<Datum, Date>((d: Datum) => new Date(d.time)).left;
interface Props {
  deviceId: string;
}

export interface Datum {
  time: Date;
  co2: number;
  temp: number;
  hum: number;
}

type TooltipData = Datum;

export type TimelineProps = {
  deviceId: string;
  data: Datum[];
  width: number;
  height: number;
  margin?: { top: number; right: number; bottom: number; left: number };
};

export const Timeline = withTooltip<TimelineProps, TooltipData>(
  ({
    deviceId,
    data,
    width,
    height,
    margin = { top: 0, right: 0, bottom: 0, left: 0 },
    showTooltip,
    hideTooltip,
    tooltipData,
    tooltipTop = 0,
    tooltipLeft = 0,
  }: TimelineProps & WithTooltipProvidedProps<TooltipData>) => {
    // bounds
    const xMax = width - margin.left - margin.right;
    const yMax = height - margin.top - margin.bottom;

    // scales
    const dateScale = React.useMemo(
      () =>
        scaleTime({
          range: [0, xMax],
          domain: extent(data, getDate) as [Date, Date],
        }),
      [xMax, data],
    );
    const stockValueScale = React.useMemo(
      () =>
        scaleLinear({
          range: [yMax, 0],
          domain: [0, (max(data, getValue) || 0) + yMax / 3],
          nice: true,
        }),
      [yMax, data],
    );

    // tooltip handler
    const handleTooltip = React.useCallback(
      (event: React.TouchEvent<SVGRectElement> | React.MouseEvent<SVGRectElement>) => {
        const { x } = localPoint(event) || { x: 0 };
        const x0 = dateScale.invert(x);
        const index = bisectDate(data, x0, 1);
        const d0 = data[index - 1];
        const d1 = data[index];
        let d = d0;
        if (d1 && getDate(d1)) {
          d = x0.valueOf() - getDate(d0).valueOf() > getDate(d1).valueOf() - x0.valueOf() ? d1 : d0;
        }
        showTooltip({
          tooltipData: d,
          tooltipLeft: x,
          tooltipTop: stockValueScale(getValue(d)),
        });
      },
      [showTooltip, stockValueScale, dateScale],
    );

    if (width < 10) return null;

    return (
      <div>
        <svg width={width} height={height}>
          <rect x={0} y={0} width={width} height={height} fill="url(#area-background-gradient)" rx={14} />
          <LinearGradient id="area-background-gradient" from={background} to={background2} />
          <LinearGradient id="area-gradient" from={accentColor} to={accentColor} toOpacity={0.1} />
          <GridRows
            scale={stockValueScale}
            width={xMax}
            strokeDasharray="3,3"
            stroke={accentColor}
            strokeOpacity={0.3}
            pointerEvents="none"
          />
          <GridColumns
            scale={dateScale}
            height={yMax}
            strokeDasharray="3,3"
            stroke={accentColor}
            strokeOpacity={0.3}
            pointerEvents="none"
          />
          <AreaClosed<Datum>
            data={data}
            x={(d: Datum) => dateScale(getDate(d))}
            y={(d: Datum) => stockValueScale(getValue(d))}
            yScale={stockValueScale}
            strokeWidth={1}
            stroke="url(#area-gradient)"
            fill="url(#area-gradient)"
            curve={curveMonotoneX}
          />
          <Bar
            x={0}
            y={0}
            width={width}
            height={height}
            fill="transparent"
            rx={14}
            onTouchStart={handleTooltip}
            onTouchMove={handleTooltip}
            onMouseMove={handleTooltip}
            onMouseLeave={() => hideTooltip()}
          />
          {tooltipData && (
            <g>
              <Line
                from={{ x: tooltipLeft, y: 0 }}
                to={{ x: tooltipLeft, y: yMax }}
                stroke={accentColorDark}
                strokeWidth={2}
                pointerEvents="none"
                strokeDasharray="5,2"
              />
              <circle
                cx={tooltipLeft}
                cy={tooltipTop + 1}
                r={4}
                fill="black"
                fillOpacity={0.1}
                stroke="black"
                strokeOpacity={0.1}
                strokeWidth={2}
                pointerEvents="none"
              />
              <circle
                cx={tooltipLeft}
                cy={tooltipTop}
                r={4}
                fill={accentColorDark}
                stroke="white"
                strokeWidth={2}
                pointerEvents="none"
              />
            </g>
          )}
        </svg>
        {tooltipData && (
          <div>
            <Tooltip top={tooltipTop - 12} left={tooltipLeft + 12} style={tooltipStyles}>
              {`${getValue(tooltipData)} ppm`}
            </Tooltip>
            <Tooltip
              top={yMax - 14}
              left={tooltipLeft}
              style={{
                ...defaultStyles,
                minWidth: 72,
                textAlign: "center",
                transform: "translateX(-50%)",
              }}
            >
              {formatDate(getDate(tooltipData))}
            </Tooltip>
          </div>
        )}
      </div>
    );
  },
);
