package de.lhtechnologies.RouteManager;

import java.util.ArrayList;

/**
 * Created by ludger on 28.02.16.
 */
public class RouteManager {
    ArrayList<Waypoint> smoothRoute;
    private int currentTargetIndex;

    public RouteManager() {
        smoothRoute = new ArrayList<>();
    }

    public void clearRoute() {
        if(smoothRoute != null) {
            smoothRoute.clear();
        }
    }

    public void addPoint(Waypoint waypoint) {
        smoothRoute.add(waypoint);
    }

    public void replaceRoute(Waypoint[] newRoute) {
        clearRoute();
        for(Waypoint wp: newRoute) {
            smoothRoute.add(wp);
        }
        currentTargetIndex = -1;
    }

    public void getNewHeading(double lat, double lon, double heading) {
        //If we do not know which waypoint is the current target, find the next waypoint and set its index
        if(currentTargetIndex == -1) {

        }

        //Find the waypoints towards which we have the lowest
    }
}
