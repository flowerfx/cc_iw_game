package org.cocos2dx.cpp.googlegameservices;

import java.text.NumberFormat;
import java.util.Collection;
import java.util.Locale;
import java.util.NavigableMap;
import java.util.TreeMap;
import java.util.Map.Entry;


public class Utility {

	public final static NumberFormat NUMBER_FORMAT = NumberFormat
			.getNumberInstance(Locale.US);

	public static boolean isNullOrEmpty(String s) {
		return (s == null) || (s.length() == 0);
	}

	public static <T> boolean isNullOrEmpty(Collection<T> c) {
		return (c == null) || (c.size() == 0);
	}

//	/**
//	 * Parse Money sang tien dang , 1K , 10M , 100B , 1.5M . . .
//	 *
//	 * @param value
//	 * @return
//	 */
//	public static String getShortMoney(long value) {
//		if (value == Long.MIN_VALUE)
//			return getShortMoney(Long.MIN_VALUE + 1);
//		if (value < 0)
//			return "-" + getShortMoney(-value);
//		if (value < 1000)
//			return Long.toString(value); // deal with easy case
//
//		Entry<Long, String> e = suffixes.floorEntry(value);
//		Long divideBy = e.getKey();
//		String suffix = e.getValue();
//
//		long truncated = value / (divideBy / 100); // the number part of the
//													// output times 10
//		boolean hasDecimal = truncated < 1000
//				&& (truncated / 100d) != (truncated / 100);
//		return hasDecimal ? (truncated / 100d) + suffix : (truncated / 100)
//				+ suffix;
//	}

	/**
	 * Convert một số kiểu long thành chuỗi có dấu chấm phân cách.
	 *
	 * @param n : Số cần chuyển
	 * @return Chuỗi thể hiện la so them dau cham.
	 */
	public static String formatNumber_dot(long n) {
		NUMBER_FORMAT.setMaximumFractionDigits(0);
		String s = NUMBER_FORMAT.format(n);
		return s.replace(',', '.');
	}

	public static boolean isnumber(String st) {
		try {
			int i = Integer.parseInt(st);
			return true;
		} catch (Exception e) {
			// TODO: handle exception
		}
		return false;
	}

}