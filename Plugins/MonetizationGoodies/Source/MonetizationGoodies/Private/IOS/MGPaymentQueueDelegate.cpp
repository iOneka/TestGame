// Copyright (c) 2021 Nineva Studios

#include "MGPaymentQueueDelegate.h"

@implementation MGPaymentQueueDelegate

- (BOOL)paymentQueue:(SKPaymentQueue *)paymentQueue shouldContinueTransaction:(SKPaymentTransaction *)transaction inStorefront:(SKStorefront *)newStorefront API_AVAILABLE(ios(13)) {
	if (_shouldContinueTransactionDelegate == nil) {
		return true;
	}
	
	return _shouldContinueTransactionDelegate(transaction, newStorefront);
}

- (BOOL)paymentQueueShouldShowPriceConsent:(SKPaymentQueue *)paymentQueue API_AVAILABLE(ios(13)) {
	if (_shouldShowPriceConsentDelegate == nil) {
		return true;
	}
	
	return _shouldShowPriceConsentDelegate();
}

@end
