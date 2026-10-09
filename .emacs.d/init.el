; -*- lexical-binding: t; -*-
(setq inhibit-startup-screen t)
(tool-bar-mode 0)
(menu-bar-mode 0)
(scroll-bar-mode 0)
(ido-mode 1)
(setq ido-enable-flex-matching t)
(setq ido-everywhere t)
(global-display-line-numbers-mode 1)
(setq-default display-line-numbers-type 'relative)
(set-face-attribute 'default nil
                    :family "CodeNewRomanNerdFont"
                    :height 150
                    :width 'regular)
(setq c-ts-indent-offset 4)

;; Change origin
;; (setq package-archives '(("gnu" . "https://mirrors.tuna.tsinghua.edu.cn/elpa/gnu/")
;;                          ("nongnu" . "https://mirrors.tuna.tsinghua.edu.cn/elpa/nongnu/")))

;; Install slang-mode
(unless (package-installed-p 'slang-mode)
	(package-vc-install
	 '(slang-mode :url "https://github.com/K1ngst0m/slang-mode.git")))
(require 'slang-mode)
(add-to-list 'auto-mode-alist '("\\.slang\\'" . slang-mode))

;; Install color-theme-sanityinc-tomorrow
(unless (package-installed-p 'color-theme-sanityinc-tomorrow)
  (package-vc-install
   '(color-theme-sanityinc-tomorrow :url "https://github.com/purcell/color-theme-sanityinc-tomorrow.git"))
  )
(load-theme 'sanityinc-tomorrow-night t)

;;Install smex
(unless (package-installed-p 'smex)
    (package-vc-install
     '(smex :url "https://github.com/nonsequitur/smex.git"))
    )
(require 'smex)
(smex-initialize)
(global-set-key (kbd "M-x") 'smex)
(global-set-key (kbd "M-X") 'smex-major-mode-commands)

;; Install company-mode
(unless (package-installed-p 'company-mode)
  (package-vc-install 
   '(company-mode :url "https://github.com/company-mode/company-mode.git")))
(require 'company)
(add-hook 'after-init-hook 'global-company-mode)
(setq company-idle-delay nil)
(global-set-key (kbd "M-TAB") 'company-complete)

;; Install magit 
(unless (package-installed-p 'magit)
  (package-vc-install
   '(magit :url "https://github.com/magit/magit.git")))
(require 'magit)

;; Install multiple-cursor 
(unless (package-installed-p 'multiple-cursors)
  (package-vc-install
   '(multiple-cursors :url "https://github.com/magnars/multiple-cursors.el.git")))
(require 'multiple-cursors)
(global-set-key (kbd "C-S-c C-S-c") 'mc/edit-lines)
(global-set-key (kbd "C->") 'mc/mark-next-like-this)
(global-set-key (kbd "C-<") 'mc/mark-previous-like-this)
(global-set-key (kbd "C-c C-<") 'mc/mark-all-like-this)

(setq treesit-language-source-alist
      '((bash "https://github.com/tree-sitter/tree-sitter-bash")
	(c "https://github.com/tree-sitter/tree-sitter-c")
	(c++ "https://github.com/tree-sitter/tree-sitter-cpp")
	(cmake "https://github.com/uyha/tree-sitter-cmake")
	(css "https://github.com/tree-sitter/tree-sitter-css")
	(html "https://github.com/tree-sitter/tree-sitter-html")
	(json "https://github.com/tree-sitter/tree-sitter-json")
	(toml "https://github.com/tree-sitter/tree-sitter-toml")
	(yaml "https://github.com/ikatyang/tree-sitter-yaml")))

(setq major-mode-remap-alist
      '((yaml-mode . yaml-ts-mode)
	(html-mode . html-ts-mode)
	(sh-mode . bash-ts-mode)
	(c-mode . c-ts-mode)
	(c++-mode . c++-ts-mode)
	(json-mode . json-ts-mode)
	(cmake-mode . cmake-ts-mode)
	(css-mode . css-ts-mode)))

(customize-set-variable 'treesit-font-lock-level 4)

(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs
               '((c-ts-mode c++-ts-mode) . ("clangd")))
  (add-to-list 'eglot-server-programs
               '(bash-ts-mode . ("bash-language-server")))
  (add-to-list 'eglot-server-programs
               '(cmake-ts-mode . ("cmake-language-server")))
  (add-to-list 'eglot-server-programs
               '(yaml-ts-mode . ("yaml-language-server")))
  (add-to-list 'eglot-server-programs
               '(json-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(css-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(html-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(slang-mode . ("slangd")))
  )

(add-hook 'c-ts-mode-hook 'eglot-ensure)
(add-hook 'c++-ts-mode-hook 'eglot-ensure)
(add-hook 'cmake-ts-mode-hook 'eglot-ensure)
(add-hook 'css-ts-mode-hook 'eglot-ensure)
(add-hook 'html-ts-mode-hook 'eglot-ensure)
(add-hook 'json-ts-mode-hook 'eglot-ensure)
(add-hook 'yaml-ts-mode-hook 'eglot-ensure)
(add-hook 'slang-mode-hook 'eglot-ensure)

(add-hook 'eglot-managed-mode-hook
          (lambda ()
            (eglot-inlay-hints-mode 0)))

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(package-selected-packages '(magit multiple-cursors))
 '(package-vc-selected-packages
   '((company-mode :url
		   "https://github.com/company-mode/company-mode.git")
     (smex :url "https://github.com/nonsequitur/smex.git")
     (color-theme-sanityinc-tomorrow :url
				     "https://github.com/purcell/color-theme-sanityinc-tomorrow.git")
     (slang-mode :url "https://github.com/K1ngst0m/slang-mode.git"))))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )
(put 'dired-find-alternate-file 'disabled nil)
