// Copyright (c) 2021 Nineva Studios

#include "MGTransactionObserverDelegate.h"

@implementation MGTransactionObserverDelegate

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
	if (_onTransactionsUpdated == nil) {
		return;
	}
	
	_onTransactionsUpdated(transactions);
}

- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
	if (_onTransactionsRemoved == nil) {
		return;
	}
	
	_onTransactionsRemoved(transactions);
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error {
	if (_onRestoreTransactionsError == nil) {
		return;
	}
	
	_onRestoreTransactionsError(error);
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
	if (_onRestoreTransactionsCompleted == nil) {
		return;
	}
	
	_onRestoreTransactionsCompleted();
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedDownloads:(NSArray<SKDownload *> *)downloads {
	if (_onDownloadsUpdated == nil) {
		return;
	}
	
	_onDownloadsUpdated(downloads);
}

- (BOOL)paymentQueue:(SKPaymentQueue *)queue shouldAddStorePayment:(SKPayment *)payment forProduct:(SKProduct *)product {
	if (_shouldAddStorePaymentForProduct == nil) {
		return true;
	}
	
	return _shouldAddStorePaymentForProduct(payment, product);
}

- (void)paymentQueue:(SKPaymentQueue *)queue didRevokeEntitlementsForProductIdentifiers:(NSArray<NSString*> *)productIdentifiers {
	if (_onEntitlementsRevoked == nil) {
		return;
	}
	
	_onEntitlementsRevoked(productIdentifiers);
}

- (void)paymentQueueDidChangeStorefront:(SKPaymentQueue *)queue {
	if (_onStoreFrontChanged == nil) {
		return;
	}
	
	_onStoreFrontChanged();
}

@end
