SELECT memid, COUNT(*) as count_bookings FROM cd.bookings
WHERE EXTRACT(HOUR FROM starttime) < 16 AND EXTRACT(HOUR FROM starttime) >= 9
GROUP BY memid
ORDER BY count_bookings;
